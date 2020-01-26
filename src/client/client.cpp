#include <iostream>
#include "central_client.h"

int main(int ac, char **av) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  central_client client("tcp://127.0.0.1:5555");

  if (!client.client_register("test", 3131)) {
    std::cout << "cannot register client" << std::endl;
  }

  std::string addr;
  uint32_t port;
  if (!client.client_lookup("test", addr))
    std::cout << "lookup failure for test" << std::endl;
  else
    std::cout << "lookup for test: " << addr << std::endl;

  if (!client.client_lookup("wrong_test", addr))
    std::cout << "lookup failure for wrong_test" << std::endl;
  else
    std::cout << "lookup for wrong_test: " << addr << std::endl;


  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}