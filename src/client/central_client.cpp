#include "central_client.h"
#include <iostream>

central_client::central_client(std::string const& address)
  : _zmq{address, zmq_helper::requester} {
}

central_client::~central_client() {
}

auto encode_send_and_read_response = [](zmq_helper &_zmq, central::central_msg& query, central::central_msg &msg) ->  bool {
  std::string output;
  //serialize and send it
  if (query.SerializeToString(&output)) {
    _zmq.send(output);
  } else {
    std::cout << "cannot create client request" << std::endl;
    return false;
  }

  //  Get the reply.
  std::string rpl;
  _zmq.recv(rpl);

  if (!msg.ParseFromString(rpl)) {
    std::cout << "cannot parse response pb" << std::endl;
    return false;
  }

  return true;
};

bool central_client::client_register(std::string const& name, uint32_t port) {
  //build register request
  central::central_msg query;
  central::client_id *id(new central::client_id());
  central::client_information *infos(new central::client_information());

  id->set_name(name);
  infos->set_port(port);
  infos->set_allocated_id(id);

  query.set_allocated_cl_register(infos);

  central::central_msg msg;

  if (!encode_send_and_read_response(_zmq, query, msg))
    return false;

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
  std::string& addr) {
  central::central_msg query;
  central::client_id *id = new central::client_id;

  id->set_name(name);
  query.set_allocated_cl_lookup(id);

  central::central_msg msg;

  if (!encode_send_and_read_response(_zmq, query, msg))
    return false;

  if(msg.commands_case() != central::central_msg::kClLookupRply) {
    std::cout << "server does not reply a response pb" << std::endl;
    return false;
  }

  if (!msg.cl_lookup_rply().response().success()) {
    std::cout << "server error on lookup: "
              << msg.cl_lookup_rply().response().error_message() << std::endl;
    return false;
  }

  addr = msg.cl_lookup_rply().client_addr();
  return true;
}

bool central_client::client_unregister(std::string const& name) {
  central::central_msg query;
  central::client_id *id = new central::client_id;

  id->set_name(name);
  query.set_allocated_cl_unregister(id);

  central::central_msg msg;

  if (!encode_send_and_read_response(_zmq, query, msg))
    return false;
  if(msg.commands_case() != central::central_msg::kClUnregisterRply) {
    std::cout << "server does not reply a response pb" << std::endl;
    return false;
  }

  if (!msg.cl_unregister_rply().success()) {
    std::cout << "server error on register: "
              << msg.cl_unregister_rply().error_message() << std::endl;
    return false;
  }

  return true;
}

void central_client::stop_server() {
  std::string stop{"STOP"};
  _zmq.send(stop);
}
