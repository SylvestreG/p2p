//
// Created by syl on 1/27/20.
//

#include "utils.h"
#include <algorithm>

std::vector<std::string> split(std::string const &str) {
  std::vector<std::string> retval;
  char sep{' '};
  size_t pos = 0, new_pos;

  while (pos != std::string::npos) {
    new_pos = str.find(sep, pos);
    if (new_pos != std::string::npos) {
      retval.push_back(str.substr(pos, new_pos - pos));
      pos = new_pos + 1;
    } else {
      retval.push_back(str.substr(pos));
      pos = new_pos;
    }
  }

  retval.erase(
      std::remove_if(retval.begin(), retval.end(),
                     [](std::string &str) -> bool { return str.empty(); }),
      retval.end());

  return retval;
}