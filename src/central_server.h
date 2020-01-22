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
using grpc::ServerContext;
using grpc::Status;
using p2p::bool_msg;
using p2p::central_server;
using p2p::client_id;
using p2p::client_info_response;
using p2p::client_information;

// Client impl
class central_server_client {
public:
  central_server_client(std::shared_ptr<Channel> channel)
      : _stub(central_server::NewStub(channel)) {}

  bool client_register(std::string const& name, std::string const& add,
                       uint16_t port);
  bool client_lookup(std::string const& name, std::string& addr,
                     std::uint16_t& port);

private:
  std::unique_ptr<central_server::Stub> _stub;
};

// Server impl
class central_server_server final : public central_server::Service {
  Status client_register(ServerContext* context,
                         client_information const* request,
    bool_msg *reply) override;

  Status client_lookup(ServerContext* context,
                       client_id const* request,
                       client_info_response *reply) override;
};

#endif // P2P_CLIENT_CENTRAL_IFACE_H_
