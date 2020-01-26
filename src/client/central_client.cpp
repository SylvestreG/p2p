#include "central_client.h"

central_client::central_client(std::string const& address)
  : _context{1}, _socket{_context, ZMQ_REQ} {
  _socket.connect(address);
}


auto encode_send_and_read_response = [](zmq::socket_t &sock, central::central_msg& query, central::central_msg &msg) ->  bool {
  std::string output;
  //serialize and send it
  if (query.SerializeToString(&output)) {

    zmq::message_t request(output.size());
    memcpy(request.data(), output.c_str(), output.size());
    sock.send(request);
  } else {
    std::cout << "cannot create client request" << std::endl;
    return false;
  }

  //  Get the reply.
  zmq::message_t reply;
  sock.recv(&reply);

  std::string
    rpl = std::string(static_cast<char *>(reply.data()), reply.size());

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

  if (!encode_send_and_read_response(_socket, query, msg))
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

  if (!encode_send_and_read_response(_socket, query, msg))
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
