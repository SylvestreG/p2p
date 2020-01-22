#include <iostream>
#include <json11.hpp>
#include "central_server.h"

using grpc::Server;
using grpc::ServerBuilder;

int main() {
  central_server_server *central = new central_server_server;

  ServerBuilder builder;
  builder.AddListeningPort("0.0.0.0:4242", grpc::InsecureServerCredentials());
  builder.RegisterService(central);
    auto server(builder.BuildAndStart());

  server->Wait();
  return 0;
}
