//
// Created by syl on 1/22/20.
//

#ifndef P2P_CLIENT_CENTRAL_IFACE_H_
#define P2P_CLIENT_CENTRAL_IFACE_H_

#include <iostream>
#include <memory>
#include <string>

#include "comm.grpc.pb.h"
#include <grpc++/grpc++.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using p2p::bool_msg;
using p2p::central_server;
using p2p::client_id;
using p2p::client_info_response;
using p2p::client_information;

class central_server_client {
public:
  central_server_client(std::shared_ptr<Channel> channel)
      : _stub(central_server::NewStub(channel)) {}

  bool client_register(std::string const &name, std::string const &add,
                       uint16_t port);
  bool client_lookup(std::string const &name, std::string &addr,
                     std::uint16_t &port);

private:
  std::unique_ptr<central_server::Stub> _stub;
};

#endif // P2P_CLIENT_CENTRAL_IFACE_H_
