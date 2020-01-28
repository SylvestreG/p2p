#include <atomic>
#include <memory>
#include <replxx.hxx>
#include <thread>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>
#include "central_client.h"
#include "cli.h"
#include "cli_cmd.h"
#include "p2p.pb.h"
#include "../zmq_helper.h"

std::shared_ptr<spdlog::logger> logger;

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
  zmq_helper _zmq{get_addr(port, true), zmq_helper::requester};

  p2p::p2p_msg req;
  req.set_exit(true);

  std::string buf;
  req.SerializeToString(&buf);
  _zmq.send(buf);
};

void p2p_bind(std::atomic_bool &init_done, cli &shell, uint32_t &port) {
  zmq_helper _zmq{get_addr(port), zmq_helper::replier};
  bool should_exit{false};

  while (true) {

    init_done = true;
    //  Wait for next request from client
    std::string req;
    _zmq.recv(req);

    if (req == "STOP") {
      break;
    }

    p2p::p2p_msg msg;
    if (msg.ParseFromString(req)) {
      switch (msg.commands_case()) {
        case p2p::p2p_msg::kMsg:
          shell.write_msg(msg.msg().name(), msg.msg().data());
          break;
        case p2p::p2p_msg::kExit:
          should_exit = true;
          break;
        default:
          logger->error("request not supported");
          break;
      }
    } else {
      logger->error("bad pbuf");
    }

    p2p::p2p_msg r;

    r.set_exit(true);
    std::string buffer;
    if (r.SerializeToString(&buffer)) {
      _zmq.send(buffer);
    }

    if (should_exit)
      break;
  }
}

int main(int ac, char **av) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  std::string name;
  std::string central;
  uint32_t port{0};

  logger = spdlog::stdout_color_mt("global");

  auto arg = lyra::cli_parser() |
    lyra::opt(name, "name")["-n"]["--name"]("p2p name") |
    lyra::opt(port, "port")["-p"]["--port"]("p2p port") |
    lyra::opt(central, "central")["-c"]["--central"]("central addr");

  auto result = arg.parse({ac, av});
  if (!result) {
    logger->error(result.errorMessage());
    logger->info("usage: ./p2p -n name -p local_binding_port -c tcp://centraladdr:central_port");
    return EXIT_FAILURE;
  }

  if (central.empty() || port == 0 || name.empty()) {
    logger->info("usage: ./p2p -n name -p local_binding_port -c tcp://centraladdr:central_port");
    return EXIT_FAILURE;
  }

  std::atomic_bool init_done{false};

  std::shared_ptr<central_client> client;
  try {
    client = std::make_shared<central_client>(central);
  } catch (std::error_code const &ec) {
    logger->error("cannot access central");
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
    logger->error("cannot register client");
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
