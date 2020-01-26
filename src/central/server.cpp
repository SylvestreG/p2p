#include "central.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  central_server server{"tcp://0.0.0.0:5555"};

  server.run();

  google::protobuf::ShutdownProtobufLibrary();
}
