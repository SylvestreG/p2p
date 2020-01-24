#include <iostream>
#include "central.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  central_client client("tcp://localhost:5555");

  //  Do 10 requests, waiting each time for a response
  for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
    client.send("Hello");
  }

  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}