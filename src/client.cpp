#include <iostream>
#include "central_server.h"

int main() {
  central_server_client client(grpc::CreateChannel(
    "localhost:50051", grpc::InsecureChannelCredentials()));

  if (client.client_register("ponay", "localhost", 4242));
    std::cout << ("registering done");

  return 0;
}
