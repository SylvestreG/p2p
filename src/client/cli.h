//
// Created by syl on 1/26/20.
//

#ifndef P2P_SRC_CLIENT_CLI_H_
#define P2P_SRC_CLIENT_CLI_H_
#include <functional>
#include <replxx.hxx>
#include <thread>
#include <unordered_map>
#include <vector>

class cli {
 public:
  typedef void (*command_fn)();

 private:
  replxx::Replxx _rx;
  std::string _history_file;
  std::string _prompt;

  std::vector<std::string> _msgs;

  std::vector<std::string> _keywords;
  std::unordered_map<std::string, std::function<void (replxx::Replxx &_rx, std::string const& input)>> _commands;

 public:
  cli();
  ~cli();

  void write_msg(std::string const& src, std::string const& msg);
  void add_command(std::string const& command_name, std::function<void (replxx::Replxx &_rx, std::string const& input)> fn);
  void run();
};

#endif //P2P_SRC_CLIENT_CLI_H_
