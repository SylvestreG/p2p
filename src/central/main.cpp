#include "central.h"
#include <lyra/lyra.hpp>

int main(int ac, char **av) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  uint16_t port;

  auto arg = lyra::cli_parser() |
    lyra::opt(port, "port")["-p"]["--port"]("port");

  auto result = arg.parse({ac, av});
  if (!result) {
    std::cerr <<  "error: " << result.errorMessage()
              << std::endl;
    std::cerr << "usage: ./central -p binding_port" << std::endl;
    return EXIT_FAILURE;
  }

  if (port == 0) {
    std::cerr << "usage: ./central -p binding_port" << std::endl;
    return EXIT_FAILURE;
  }

  std::string bind_addr{"tcp://*:"};
  bind_addr.append(std::to_string(port));
  std::cout << "binding on " << bind_addr << std::endl;

  central_server server{bind_addr};

  server.run();

  google::protobuf::ShutdownProtobufLibrary();
  return EXIT_SUCCESS;
}
