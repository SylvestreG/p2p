//
// Created by syl on 1/22/20.
//

#include "central_server.h"

bool central_server_client::client_register(std::string const &name,
                                            std::string const &add,
                                            uint16_t port) {
  client_information info;
  client_id *id = new client_id();
  id->set_name(name);
  info.set_allocated_id(id);
  info.set_addr(add);
  info.set_port(port);

  bool_msg success;
  ClientContext context;

  Status status = _stub->client_register(&context, info, &success);

  // Act upon its status.
  if (status.ok()) {
    return success.success();
  } else {
    return false;
  }
};

bool central_server_client::client_lookup(std::string const &name,
                                          std::string &addr,
                                          std::uint16_t &port) {
  client_id id;
  id.set_name(name);

  client_info_response response;
  ClientContext context;

  Status status = _stub->client_lookup(&context, id, &response);

  // Act upon its status.
  if (status.ok() && response.found()) {
    addr = response.data().addr();
    port = static_cast<uint16_t>(response.data().port());
    return true;
  }

  return false;
};