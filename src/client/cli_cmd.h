//
// Created by syl on 1/27/20.
//

#ifndef P2P_SRC_CLIENT_CLI_CMD_H_
#define P2P_SRC_CLIENT_CLI_CMD_H_

#include <replxx.hxx>
#include "central_client.h"

void p2p_send_msg(
  std::shared_ptr<central_client> client,
  std::string const& name,
  replxx::Replxx& rx,
  std::string const& input);
void clear(replxx::Replxx& rx, std::string const& input);
void history(replxx::Replxx& rx, std::string const& input);
void help(replxx::Replxx& rx, std::string const& input);

#endif // P2P_SRC_CLIENT_CLI_CMD_H_
