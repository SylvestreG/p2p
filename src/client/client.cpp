#include <iomanip>
#include <iostream>
#include <memory>
#include <replxx.hxx>
#include "central_client.h"
#include "cli.h"

using Replxx = replxx::Replxx;

int main(int ac, char **av) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::unique_ptr<central_client> client;
  try {
    client.reset(new central_client("tcp://127.0.0.1:5555"));
  } catch (std::error_code const& ec) {
    std::cerr << "\x1b[1;32merror\x1b[0m" << ": cannot access to central"
              << std::endl;
    return EXIT_FAILURE;
  }

  if (!client->client_register("test", 3131)) {
    std::cout << "cannot register client" << std::endl;
    return EXIT_FAILURE;
  }

  cli shell;

  shell.add_command("clear", [](replxx::Replxx &rx) { rx.clear_screen(); });
  shell.add_command("history", [](replxx::Replxx &rx) {
    Replxx::HistoryScan hs( rx.history_scan() );
    for ( int i( 0 ); hs.next(); ++ i ) {
      std::cout << std::setw(4) << i << ": " << hs.get().text() << "\n";
    }
  });
  shell.add_command("help", [](replxx::Replxx &rx) {
    std::cout << "help\n\tdisplays the help output\n"
    << "quit\n\texit the repl\n"
    << "exit\n\texit the repl\n"
    << "clear\n\tclears the screen\n"
    << "history\n\tdisplays the history output\n"
    << "send peername msg\n\tsend msg to peername" << std::endl;
  });

  shell.run();

  std::string addr;
  uint32_t port;
  if (!client->client_lookup("test", addr))
    std::cout << "lookup failure for test" <<
              std::endl;
  else
    std::cout << "lookup for test: " << addr <<
              std::endl;

  if (!client->client_lookup("wrong_test", addr))
    std::cout << "lookup failure for wrong_test" <<
              std::endl;
  else
    std::cout << "lookup for wrong_test: " << addr <<
              std::endl;

  google::protobuf::ShutdownProtobufLibrary();

  return EXIT_SUCCESS;
}