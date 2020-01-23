#include "central.h"

int main() {
  central_server server{"tcp://0.0.0.0:5555"};

  server.run();
}
