//
// Created by syl on 1/26/20.
//

#ifndef P2P_SRC_CLIENT_CLI_H_
#define P2P_SRC_CLIENT_CLI_H_

#include <replxx.hxx>
#include <unordered_map>
#include <vector>

class cli {
 public:
  typedef void (*command_fn)(replxx::Replxx &_rx);

 private:
  replxx::Replxx _rx;
  std::string _history_file;
  std::string _prompt;

  std::vector<std::string> _keywords;
  std::unordered_map<std::string, command_fn> _commands;

 public:
  cli();
  ~cli();

  void add_command(std::string const& command_name, command_fn fn);
  void run();
};

#endif //P2P_SRC_CLIENT_CLI_H_
