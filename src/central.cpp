//
// Created by syl on 1/22/20.
//

#include "central.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "comm.pb.h"

central_client::central_client(std::string const& address)
  : _context{1}, _socket{_context, ZMQ_REQ} {
  _socket.connect(address);
}

void central_client::send(std::string const& msg) {

}

bool central_client::client_register(std::string const& name, uint32_t port) {
  //build register request
  central::central_msg query;
  central::client_id *id(new central::client_id());
  central::client_information *infos(new central::client_information());

  id->set_name(name);
  infos->set_port(port);
  infos->set_allocated_id(id);

  query.set_allocated_cl_register(infos);

  std::string output;

  //serialize and send it
  if (query.SerializeToString(&output)) {

    zmq::message_t request(output.size());
    memcpy(request.data(), output.c_str(), output.size());
    _socket.send(request);
  } else {
    std::cout << "cannot create client register request" << std::endl;
    return false;
  }

  //  Get the reply.
  zmq::message_t reply;
  _socket.recv(&reply);

  std::string
    rpl = std::string(static_cast<char *>(reply.data()), reply.size());
  central::central_msg msg;
  if (!msg.ParseFromString(rpl)) {
    std::cout << "cannot parse response pb" << std::endl;
    return false;
  }
  if(msg.commands_case() != central::central_msg::kClRegisterRply) {
    std::cout << "server does not reply a response pb" << std::endl;
    return false;
  }

  if (!msg.cl_register_rply().success()) {
    std::cout << "server error on register: "
              << msg.cl_register_rply().error_message() << std::endl;
    return false;
  }

  return true;
}

bool central_client::client_lookup(
  std::string const& name,
  std::string& addr,
  uint32_t& port) {

}

central_server::central_server(std::string const& addr)
  : _context{1}, _socket{_context, ZMQ_REP} {
  //  Prepare our context and socket
  _socket.bind("tcp://*:5555");
}

zmq::message_t central_server::_handle_client_register(
  central::client_information const& infos,
  std::string const& peer) {
  std::string const& client{infos.id().name()};
  std::cout << "registring client " << client << " (tcp://:" << peer
            << ":" << infos.port() << ") : ";

  central::central_msg msg;
  central::client_generic_response *response = new central::client_generic_response;


  //build client addr
  std::stringstream ss_client;
  ss_client << "tcp://";
  ss_client << peer;
  ss_client << ":" << infos.port();
  std::cout << ss_client.str() << std::endl;


  std::string err;
  // register client for the 1st time;
  if (_client_map.find(client) == _client_map.end())
    _client_map[client] = ss_client.str();
  // another client already registred;
  else if (_client_map[client] != ss_client.str())
    err = "another client is already registred for this name";

  if (err.empty()) {
    response->set_success(true);
    std::cout << "success" << std::endl;
  } else {
    response->set_success(false);
    response->set_error_message(err);
    std::cout << "error" << std::endl;
  }
  msg.set_allocated_cl_register_rply(response);

  std::string buffer;
  msg.SerializeToString(&buffer);

  zmq::message_t reply(buffer.size());
  memcpy(reply.data(), buffer.c_str(), buffer.size());
  return reply;
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
        std::cout << "lookup" << std::endl;
        break;

      default:
        std::cout << "cannot parse msg" << std::endl;
        break;
    }

    //  Send reply back to client
    _socket.send(reply);
  }
}
