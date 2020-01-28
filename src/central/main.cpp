#include "central.h"
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

std::shared_ptr<spdlog::logger> logger;

int main(int ac, char **av) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  uint16_t port;

  logger = spdlog::stdout_color_mt("global");

  auto arg =
      lyra::cli_parser() | lyra::opt(port, "port")["-p"]["--port"]("port");

  auto result = arg.parse({ac, av});
  if (!result) {
    logger->error(result.errorMessage());
    logger->info("usage: ./central -p binding_port");
    return EXIT_FAILURE;
  }

  if (port == 0) {
    logger->info("usage: ./central -p binding_port");
    return EXIT_FAILURE;
  }

  std::string bind_addr{"tcp://*:"};
  bind_addr.append(std::to_string(port));
  logger->info("binding on {}", bind_addr);

  central_server server{bind_addr};

  server.run();

  google::protobuf::ShutdownProtobufLibrary();
  return EXIT_SUCCESS;
}
