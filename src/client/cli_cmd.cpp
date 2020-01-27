//
// Created by syl on 1/27/20.
//
#include "cli_cmd.h"
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include "utils.h"

std::unordered_map<std::string, std::string> p2p_proxy;

void p2p_send_msg(std::shared_ptr<central_client> client, replxx::Replxx &rx,
                  std::string const &input) {
  std::string send_args{input};

  // split all words
  std::vector<std::string> words = std::move(split(input));

  // we need at least 3 words : send addr data
  if (words.size() < 3) {
    std::cout << "error: bad format for msg\n\tusage: msg addr data"
              << std::endl;
    return;
  }

  std::string const &name{words[1]};
  std::size_t data_pos = {input.find(words[1], 5)};
  data_pos += name.size();

  std::string const &data{input.substr(
      data_pos + 1,                  // pos + 1 to get space after addr
      input.size() - data_pos - 2)}; // -2 for space after addr and \n

  std::string addr;
  if (p2p_proxy.find(name) == p2p_proxy.end()) {
    std::cout << "unknown p2p client : '" << name << "' asking central" << std::endl;
    if (!client->client_lookup(name, addr)) {
      std::cout << "error no '" << name << "' registred" << std::endl;
      return ;
    }

    p2p_proxy[name] = addr;
  }

  void *ctx = zmq_ctx_new();
  void *sock = zmq_socket(ctx, ZMQ_REQ);

  if (!ctx || !sock) {
    std::cout << "err" << std::endl;
  }

  zmq_connect(sock, p2p_proxy[name].c_str());

  zmq_msg_t msg;
  zmq_msg_init_size(&msg, data.length());
  memcpy(zmq_msg_data(&msg), data.c_str(), data.size());
  zmq_msg_send(&msg, sock, 0);
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
