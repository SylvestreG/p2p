#include "central_client.h"
#include <iostream>

central_client::central_client(std::string const& address)
  : _context{zmq_ctx_new ()}, _socket{zmq_socket(_context, ZMQ_REQ)} {
  zmq_connect(_socket, address.c_str());
}

central_client::~central_client() {
  zmq_close(_socket);
  zmq_ctx_destroy(_context);
}

static void my_free (void *data, void *hint)
{
  free(data);
}

auto encode_send_and_read_response = [](void *sock, central::central_msg& query, central::central_msg &msg) ->  bool {
  std::string output;
  //serialize and send it
  if (query.SerializeToString(&output)) {
    zmq_msg_t send;
    zmq_msg_init_size(&send, output.size());
    memcpy(zmq_msg_data(&send), output.c_str(), output.size());
    zmq_msg_send(&send, sock, 0);
  } else {
    std::cout << "cannot create client request" << std::endl;
    return false;
  }

  //  Get the reply.
  zmq_msg_t reply;
  zmq_msg_init(&reply);
  zmq_msg_recv(&reply, sock, 0);

  std::string
    rpl = std::string(static_cast<char *>(zmq_msg_data(&reply)), zmq_msg_size(&reply));

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

void central_client::stop_server() {
  zmq_send(_socket, "STOP", 4, 0);
}
