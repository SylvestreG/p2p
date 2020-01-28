//
// Created by syl on 1/27/20.
//
#include "cli_cmd.h"
#include <unordered_map>
#include <spdlog/spdlog.h>
#include "utils.h"
#include "p2p.pb.h"
#include "../zmq_helper.h"

extern std::shared_ptr<spdlog::logger> logger;

static std::unordered_map<std::string, std::string> p2p_proxy;

//command to send a msg
void p2p_send_msg(std::shared_ptr<central_client> client, std::string const& name,
  replxx::Replxx &rx, std::string const &input) {
  std::string send_args{input};

  // split all words
  std::vector<std::string> words = std::move(split(input));

  // we need at least 3 words : send addr data
  if (words.size() < 3) {
    logger->error("bad format for msg\n\tusage: msg addr data");
    return;
  }

  //get dest name and data
  std::string const &cl_name{words[1]};
  std::size_t data_pos = {input.find(words[1], 5)};
  data_pos += cl_name.size() + 1;

  std::string const &data{input.substr(
      data_pos,
      input.size() - data_pos)}; //  for space after addr and \n

  //find if dest name exist
  std::string addr;
  if (p2p_proxy.find(cl_name) == p2p_proxy.end() && cl_name != name) {
    logger->info("unknown p2p client : '{}' asking central", cl_name);

    if (!client->client_lookup(cl_name, addr)) {
      logger->error("no {} registred", cl_name);
      return ;
    }

    p2p_proxy[cl_name] = addr;
  }


  //myself ?
  if (cl_name == name) {
    logger->info("send to myself: {}", data);
    return ;
  }

  //send msg
  zmq_helper _zmq{p2p_proxy[cl_name].c_str(), zmq_helper::requester};

  p2p::p2p_msg req;
  p2p::msg_query *mq = new p2p::msg_query;
  req.set_allocated_msg(mq);
  mq->set_name(name);
  mq->set_data(data);

  std::string buf;
  if (req.SerializeToString(&buf)) {
    _zmq.send(buf);

    //answer is always ok!
    std::string rep;
    _zmq.recv(rep);
  }
}

//clear screen
void clear(replxx::Replxx &rx, std::string const &input) {
  rx.clear_screen();
}

//get history
void history(replxx::Replxx &rx, std::string const &input) {
  replxx::Replxx::HistoryScan hs(rx.history_scan());
  for (int i(0); hs.next(); ++i) {
    logger->info("{0} : {1}", i, hs.get().text());
  }
}

//help
void help(replxx::Replxx &rx, std::string const &input) {
  logger->info("\nhelp\n\tdisplays the help output\n"
               "quit\n\texit the repl\n"
               "exit\n\texit the repl\n"
               "clear\n\tclears the screen\n"
               "history\n\tdisplays the history output\n"
               "send peername msg\n\tsend msg to peername");
}
