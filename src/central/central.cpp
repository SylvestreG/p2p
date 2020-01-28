//
// Created by syl on 1/22/20.
//

#include "central.h"
#include <spdlog/spdlog.h>
#include <sstream>

extern std::shared_ptr<spdlog::logger> logger;

central_server::central_server(std::string const &addr)
    : _zmq{addr, zmq_helper::replier} {}

central_server::~central_server() {}

auto handle_response = [](std::string &err, auto &response,
                          auto &msg) -> std::string {
  if (err.empty()) {
    response->set_success(true);
    logger->info("success");
  } else {
    response->set_success(false);
    response->set_error_message(err);
    logger->error(err);
  }

  std::string buffer;
  msg.SerializeToString(&buffer);

  return buffer;
};

std::string central_server::_handle_client_register(
    central::client_information const &infos, std::string const &peer) {
  std::string const &client{infos.id().name()};
  logger->info("registring client : {0} (tcp://:{1}:{2})", client, peer,
               infos.port());

  central::central_msg msg;
  central::client_generic_response *response =
      new central::client_generic_response;
  msg.set_allocated_cl_register_rply(response);

  // build client addr
  std::stringstream ss_client;
  ss_client << "tcp://";
  ss_client << peer;
  ss_client << ":" << infos.port();

  std::string err;
  // register client for the 1st time;
  if (_client_map.find(client) == _client_map.end())
    _client_map[client] = ss_client.str();
  // another client already registred;
  else if (_client_map[client] != ss_client.str())
    err = "another client is already registred for this name";

  return handle_response(err, response, msg);
}

std::string
central_server::_handle_client_lookup(central::client_id const &id) {
  std::string const &name{id.name()};
  logger->info("looking for : {}", name);

  central::central_msg msg;
  central::client_generic_response *generic =
      new central::client_generic_response;
  central::client_lookup_response *response =
      new central::client_lookup_response;
  response->set_allocated_response(generic);
  msg.set_allocated_cl_lookup_rply(response);

  std::string err;
  if (_client_map.find(name) == _client_map.end())
    err = "unknwon client";
  else
    response->set_client_addr(_client_map[name]);

  return handle_response(err, generic, msg);
}

std::string
central_server::_handle_client_unregister(central::client_id const &id) {
  std::string const &name{id.name()};
  logger->info("unregister : {}", name);

  central::central_msg msg;
  central::client_generic_response *response =
      new central::client_generic_response;
  msg.set_allocated_cl_unregister_rply(response);

  std::string err;
  if (_client_map.find(name) == _client_map.end())
    err = "unknwon client";
  else
    _client_map.erase(name);

  return handle_response(err, response, msg);
}

void central_server::run() {
  while (true) {
    std::string req;
    std::string peer;
    //  Wait for next request from client
    _zmq.recv_and_get_peer(req, peer);

    central::central_msg query;

    if (!query.ParseFromString(req)) {
      if (req == "STOP")
        return;

      logger->error("cannot decode pb");
      continue;
    }

    std::string reply;
    switch (query.commands_case()) {
    case central::central_msg::kClRegister:
      reply = _handle_client_register(query.cl_register(), peer);
      break;

    case central::central_msg::kClLookup:
      reply = _handle_client_lookup(query.cl_lookup());
      break;

    case central::central_msg::kClUnregister:
      reply = _handle_client_unregister(query.cl_unregister());
      break;

    default:
      logger->error("cannot parse msg");
      break;
    }

    //  Send reply back to client
    _zmq.send(reply);
  }
}
