//
// Created by syl on 1/26/20.
//

#include "cli.h"
#include <cstring>
#include <iostream>

using namespace replxx;

cli::cli() : _history_file{".p2p-history"}, _prompt{"\x1b[1;32mp2p\x1b[0m> "} {
  _rx.install_window_change_handler();

  // load the history file if it exists
  _rx.history_load(_history_file);

  // set the max history size
  _rx.set_max_history_size(128);

  // set the max number of hint rows to show
  _rx.set_max_hint_rows(3);
}

cli::~cli() {
  // save the history
  _rx.history_save(_history_file);
}

static auto hook_completion =
    [](std::string const &context, int &contextLen,
       std::vector<std::string> const &examples) -> Replxx::completions_t {
  auto utf8str_codepoint_len = [](char const *s, int utf8len) -> int {
    int codepointLen = 0;
    unsigned char m4 = 128 + 64 + 32 + 16;
    unsigned char m3 = 128 + 64 + 32;
    unsigned char m2 = 128 + 64;
    for (int i = 0; i < utf8len; ++i, ++codepointLen) {
      char c = s[i];
      if ((c & m4) == m4) {
        i += 3;
      } else if ((c & m3) == m3) {
        i += 2;
      } else if ((c & m2) == m2) {
        i += 1;
      }
    }
    return (codepointLen);
  };

  auto context_len = [](char const *prefix) -> int {
    char const wb[] = " \t\n\r\v\f-=+*&^%$#@!,./?<>;:`~'\"[]{}()\\|";
    int i = strlen(prefix) - 1;
    int cl = 0;
    while (i >= 0) {
      if (strchr(wb, prefix[i]) != NULL) {
        break;
      }
      ++cl;
      --i;
    }
    return (cl);
  };

  Replxx::completions_t completions;
  int utf8ContextLen(context_len(context.c_str()));
  int prefixLen(context.length() - utf8ContextLen);
  if ((prefixLen > 0) && (context[prefixLen - 1] == '\\')) {
    --prefixLen;
    ++utf8ContextLen;
  }
  contextLen =
      utf8str_codepoint_len(context.c_str() + prefixLen, utf8ContextLen);

  std::string prefix{context.substr(prefixLen)};
  if (prefix == "\\pi") {
    completions.push_back("π");
  } else {
    for (auto const &e : examples) {
      if (e.compare(0, prefix.size(), prefix) == 0) {
        Replxx::Color c(Replxx::Color::DEFAULT);
        if (e.find("brightred") != std::string::npos) {
          c = Replxx::Color::BRIGHTRED;
        } else if (e.find("red") != std::string::npos) {
          c = Replxx::Color::RED;
        }
        completions.emplace_back(e.c_str(), c);
      }
    }
  }

  return completions;
};

void cli::write_msg(std::string const& msg) {
  _rx.emulate_key_press(13);
}

void cli::add_command(
    std::string const &command_name,
    std::function<void(Replxx &_rx, std::string const &input)> fn) {
  _keywords.push_back(command_name);
  _commands[command_name] = fn;
}

void cli::run() {
  _keywords.push_back("quit");
  _keywords.push_back("exit");
  _rx.set_completion_callback(std::bind(hook_completion, std::placeholders::_1,
                                        std::placeholders::_2,
                                        cref(_keywords)));

  for (;;) {
    // display the prompt and retrieve input from the user
    char const *cinput{nullptr};

    do {
      cinput = _rx.input(_prompt);
    } while ((cinput == nullptr) && (errno == EAGAIN));

    if (cinput == nullptr) {
      break;
    }

    // change cinput into a std::string
    // easier to manipulate
    std::string input{cinput};

    if (input.empty()) {
      // user hit enter on an empty line

      continue;

    } else if (input.compare(0, 5, "quit") == 0 ||
               input.compare(0, 5, "exit") == 0) {
      _rx.history_add(input);
      break;
    } else {
      for (auto &key : _commands)
        if (!input.compare(0, key.first.size(), key.first))
          key.second(_rx, input);
    }

    _rx.history_add(input);
  }
}
