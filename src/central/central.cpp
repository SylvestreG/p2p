//
// Created by syl on 1/22/20.
//

#include "central.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

central_server::central_server(std::string const& addr)
  : _context{zmq_ctx_new()}, _socket{zmq_socket(_context, ZMQ_REP)} {
  //  Prepare our context and socket
  zmq_bind(_socket, addr.c_str());
}

central_server::~central_server() {
  zmq_close(_socket);
  zmq_ctx_destroy(_context);
}

auto handle_response = [](std::string &err, auto& response, auto& msg) -> zmq_msg_t {
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

  zmq_msg_t reply;
  zmq_msg_init_size(&reply, buffer.size());
  memcpy(zmq_msg_data(&reply), buffer.c_str(), buffer.size());
  return reply;
};

zmq_msg_t central_server::_handle_client_register(
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

zmq_msg_t central_server::_handle_client_lookup(central::client_id const& id) {
  std::string const& name{id.name()};
  std::cout << "looking for " << name << " : ";

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

zmq_msg_t central_server::_handle_client_unregister(central::client_id const& id) {
  std::string const& name{id.name()};
  std::cout << "unregister " << name << " : ";

  central::central_msg msg;
  central::client_generic_response *response = new central::client_generic_response;
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
    zmq_msg_t request;
    zmq_msg_init(&request);

    //  Wait for next request from client
    zmq_msg_recv(&request, _socket, 0);
    std::string
      rpl = std::string(static_cast<char *>(zmq_msg_data(&request)), zmq_msg_size(&request));

    central::central_msg query;

    if (!query.ParseFromString(rpl)) {
      if (rpl == "STOP")
        return ;

      std::cout << "cannot decode pb" << std::endl;
      continue;
    }

    zmq_msg_t reply;
    switch (query.commands_case()) {
      case central::central_msg::kClRegister:
        reply = _handle_client_register(query.cl_register(),
                                zmq_msg_gets(&request, "Peer-Address"));
        break;

      case central::central_msg::kClLookup:
        reply = _handle_client_lookup(query.cl_lookup());
        break;

      case central::central_msg::kClUnregister:
        reply = _handle_client_unregister(query.cl_unregister());
        break;

      default:
        std::cout << "cannot parse msg" << std::endl;
        break;
    }

    //  Send reply back to client
    zmq_msg_send(&reply, _socket, 0);
  }
}
