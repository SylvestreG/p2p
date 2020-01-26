//
// Created by syl on 1/22/20.
//

#include "central.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

central_server::central_server(std::string const& addr)
  : _context{1}, _socket{_context, ZMQ_REP} {
  //  Prepare our context and socket
  _socket.bind("tcp://*:5555");
}

auto handle_response = [](std::string &err, auto& response, auto& msg) -> zmq::message_t {
  if (err.empty()) {
    response->set_success(true);
    std::cout << "success" << std::endl;
  } else {
    response->set_success(false);
    response->set_error_message(err);
    std::cout << "error" << std::endl;
  }

  std::string buffer;
  msg.SerializeToString(&buffer);

  zmq::message_t reply(buffer.size());
  memcpy(reply.data(), buffer.c_str(), buffer.size());
  return reply;
};

zmq::message_t central_server::_handle_client_register(
  central::client_information const& infos,
  std::string const& peer) {
  std::string const& client{infos.id().name()};
  std::cout << "registring client " << client << " (tcp://:" << peer
            << ":" << infos.port() << ") : ";

  central::central_msg msg;
  central::client_generic_response *response = new central::client_generic_response;
  msg.set_allocated_cl_register_rply(response);

  //build client addr
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

zmq::message_t central_server::_handle_client_lookup(central::client_id const& id) {
  std::string const& name{id.name()};
  std::cout << "looking for " << name << " ";

  central::central_msg msg;
  central::client_generic_response *generic = new central::client_generic_response;
  central::client_lookup_response *response = new central::client_lookup_response;
  response->set_allocated_response(generic);
  msg.set_allocated_cl_lookup_rply(response);

  std::string err;
  if (_client_map.find(name) == _client_map.end())
    err = "unknwon client";
  else
    response->set_client_addr(_client_map[name]);

  return handle_response(err, generic, msg);
}


void central_server::run() {
  while (true) {
    zmq::message_t request;

    //  Wait for next request from client
    _socket.recv(&request);
    std::string
      rpl = std::string(static_cast<char *>(request.data()), request.size());

    central::central_msg query;

    if (!query.ParseFromString(rpl)) {
      std::cout << "cannot decode pb" << std::endl;
      continue;
    }

    zmq::message_t reply;
    switch (query.commands_case()) {
      case central::central_msg::kClRegister:
        reply = _handle_client_register(query.cl_register(),
                                request.gets(
                                  "Peer-Address"));
        break;

      case central::central_msg::kClLookup:
        reply = _handle_client_lookup(query.cl_lookup());
        break;

      default:
        std::cout << "cannot parse msg" << std::endl;
        break;
    }

    //  Send reply back to client
    _socket.send(reply);
  }
}
