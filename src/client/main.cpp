#include <atomic>
#include <iostream>
#include <memory>
#include <replxx.hxx>
#include <sstream>
#include <thread>
#include <lyra/lyra.hpp>
#include "central_client.h"
#include "cli.h"
#include "cli_cmd.h"
#include "p2p.pb.h"

using Replxx = replxx::Replxx;

static auto get_addr = [](uint32_t port, bool client = false) -> std::string {
  std::stringstream ss;
  ss << "tcp://";
  if (client)
    ss << "127.0.0.1:";
  else
    ss << "*:";
  ss << port;
  return ss.str();
};

static auto send_stop = [](uint32_t port) {
  void *ctx = zmq_ctx_new();
  void *socket = zmq_socket(ctx, ZMQ_REQ);

  zmq_connect(socket, get_addr(port, true).c_str());

  p2p::p2p_msg req;
  req.set_exit(true);

  std::string buf;
  req.SerializeToString(&buf);

  zmq_msg_t msg;
  zmq_msg_init_size(&msg, buf.size());
  memcpy(zmq_msg_data(&msg), buf.c_str(), buf.size());
  zmq_msg_send(&msg, socket, 0);

  zmq_close(socket);
  zmq_ctx_destroy(ctx);
};

void p2p_bind(std::atomic_bool &init_done, cli &shell, uint32_t &port) {
  void *ctx = zmq_ctx_new();
  void *socket = zmq_socket(ctx, ZMQ_REP);

  zmq_bind(socket, get_addr(port).c_str());

  bool should_exit{false};

  while (true) {
    zmq_msg_t request;
    zmq_msg_init(&request);

    init_done = true;
    //  Wait for next request from client
    zmq_msg_recv(&request, socket, 0);
    std::string rpl = std::string(static_cast<char *>(zmq_msg_data(&request)),
                                    zmq_msg_size(&request));


    p2p::p2p_msg msg;
    if (msg.ParseFromString(rpl)) {
      switch (msg.commands_case()) {
        case p2p::p2p_msg::kMsg:
          shell.write_msg(msg.msg().name(), msg.msg().data());
          break;
        case p2p::p2p_msg::kExit:
          should_exit = true;
          break;
        default:
          std::cout << "error: not supported request" << std::endl;
          break;
      }
    } else {
      std::cout << "error: bad pb" << std::endl;
    }

    if (rpl == "STOP") {
      break;
    }

    p2p::p2p_msg r;

    r.set_exit(true);
    std::string buffer;
    if (r.SerializeToString(&buffer)) {
      zmq_msg_t reply;
      zmq_msg_init_size(&reply, buffer.size());
      memcpy(zmq_msg_data(&reply), buffer.c_str(), buffer.size());
      zmq_msg_send(&reply, socket, 0);
    }

    if (should_exit)
      break;
  }

  zmq_close(socket);
  zmq_ctx_destroy(ctx);
}

int main(int ac, char **av) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  std::string name;
  std::string central;
  uint32_t port{0};

  auto arg = lyra::cli_parser() |
    lyra::opt(name, "name")["-n"]["--name"]("p2p name") |
    lyra::opt(port, "port")["-p"]["--port"]("p2p port") |
    lyra::opt(central, "central")["-c"]["--central"]("central addr");

  auto result = arg.parse({ac, av});
  if (!result) {
    std::cerr <<  "error: " << result.errorMessage()
              << std::endl;
    std::cerr << "usage: ./p2p -n name -p local_binding_port -c tcp://centraladdr:central_port" << std::endl;
    return EXIT_FAILURE;
  }

  if (central.empty() || port == 0 || name.empty()) {
    std::cerr << "usage: ./p2p -n name -p local_binding_port -c tcp://centraladdr:central_port" << std::endl;
    return EXIT_FAILURE;
  }

  std::atomic_bool init_done{false};

  std::shared_ptr<central_client> client;
  try {
    client = std::make_shared<central_client>(central);
  } catch (std::error_code const &ec) {
    std::cerr << "\x1b[1;32merror\x1b[0m"
              << ": cannot access to central" << std::endl;
    return EXIT_FAILURE;
  }

  cli shell;

  std::thread msg_thread{
      std::bind(p2p_bind, std::ref(init_done), std::ref(shell), std::ref(port))};

  while (!init_done)
    std::this_thread::sleep_for(std::chrono::milliseconds{5});

  if (!client->client_register(name, port)) {
    send_stop(port);
    msg_thread.join();
    std::cout << "\x1b[1;32merror\x1b[0m cannot register client" << std::endl;
    return EXIT_FAILURE;
  }

  shell.add_command("clear", clear);
  shell.add_command("history", history);
  shell.add_command("help", help);
  shell.add_command("send",
                    std::bind(p2p_send_msg, client, name, std::placeholders::_1,
                              std::placeholders::_2));

  shell.run();

  client->client_unregister(name);
  send_stop(port);
  msg_thread.join();

  google::protobuf::ShutdownProtobufLibrary();

  return EXIT_SUCCESS;
}
