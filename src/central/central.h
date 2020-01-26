//
// Created by syl on 1/22/20.
//

#ifndef P2P_CENTRAL_IFACE_H_
#define P2P_CENTRAL_IFACE_H_

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <zmq.h>
#include "central.pb.h"

class central_server {
private:
  void *_context;
  void *_socket;

  std::unordered_map<std::string, std::string> _client_map; //name -> addr

  zmq_msg_t _handle_client_register(central::client_information const& infos, std::string const& peer);
  zmq_msg_t _handle_client_lookup(central::client_id const& id);

public:
  central_server() = delete;
  ~central_server();
  central_server(std::string const& addr);

  void run();
};

#endif // P2P_CENTRAL_IFACE_H_
