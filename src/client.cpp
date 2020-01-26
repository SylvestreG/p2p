#include <iostream>
#include "central.h"

int main(int ac, char **av) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  central_client client("tcp://127.0.0.1:5555");

  if (!client.client_register("test", 3131)) {
    std::cout << "cannot register client" << std::endl;
  }

  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}