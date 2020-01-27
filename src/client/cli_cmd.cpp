//
// Created by syl on 1/27/20.
//
#include "cli_cmd.h"
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include "utils.h"
#include "p2p.pb.h"

std::unordered_map<std::string, std::string> p2p_proxy;

void p2p_send_msg(std::shared_ptr<central_client> client, std::string const& name,
  replxx::Replxx &rx, std::string const &input) {
  std::string send_args{input};

  // split all words
  std::vector<std::string> words = std::move(split(input));

  // we need at least 3 words : send addr data
  if (words.size() < 3) {
    std::cout << "error: bad format for msg\n\tusage: msg addr data"
              << std::endl;
    return;
  }

  std::string const &cl_name{words[1]};
  std::size_t data_pos = {input.find(words[1], 5)};
  data_pos += cl_name.size() + 1;

  std::string const &data{input.substr(
      data_pos,
      input.size() - data_pos)}; //  for space after addr and \n

  std::string addr;
  if (p2p_proxy.find(cl_name) == p2p_proxy.end()) {
    std::cout << "unknown p2p client : '" << cl_name << "' asking central" << std::endl;
    if (!client->client_lookup(cl_name, addr)) {
      std::cout << "error no '" << cl_name << "' registred" << std::endl;
      return ;
    }

    p2p_proxy[cl_name] = addr;
  }

  void *ctx = zmq_ctx_new();
  void *sock = zmq_socket(ctx, ZMQ_REQ);

  zmq_connect(sock, p2p_proxy[cl_name].c_str());

  p2p::p2p_msg req;
  p2p::msg_query *mq = new p2p::msg_query;
  req.set_allocated_msg(mq);
  mq->set_name(name);
  mq->set_data(data);

  std::string buf;
  if (req.SerializeToString(&buf)) {
    zmq_msg_t msg;
    zmq_msg_init_size(&msg, buf.length());
    memcpy(zmq_msg_data(&msg), buf.c_str(), buf.size());
    zmq_msg_send(&msg, sock, 0);

    //answer is always ok!
    zmq_msg_t reply;
    zmq_msg_init(&reply);
    zmq_msg_recv(&reply, sock, 0);
  }
  zmq_close(sock);
  zmq_ctx_destroy(ctx);
}

void clear(replxx::Replxx &rx, std::string const &input) {
  rx.clear_screen();
}

void history(replxx::Replxx &rx, std::string const &input) {
  replxx::Replxx::HistoryScan hs(rx.history_scan());
  for (int i(0); hs.next(); ++i) {
    std::cout << std::setw(4) << i << ": " << hs.get().text() << "\n";
  }
}
void help(replxx::Replxx &rx, std::string const &input) {
  std::cout << "help\n\tdisplays the help output\n"
            << "quit\n\texit the repl\n"
            << "exit\n\texit the repl\n"
            << "clear\n\tclears the screen\n"
            << "history\n\tdisplays the history output\n"
            << "send peername msg\n\tsend msg to peername" << std::endl;
}
