//
// Created by syl on 1/22/20.
//

#ifndef P2P_CLIENT_CENTRAL_IFACE_H_
#define P2P_CLIENT_CENTRAL_IFACE_H_

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <zmq.hpp>
#include "comm.pb.h"

class central_server {
private:
  zmq::context_t _context;
  zmq::socket_t _socket;

  std::unordered_map<std::string, std::string> _client_map; //name -> addr

  zmq::message_t _handle_client_register(central::client_information const& infos, std::string const& peer);
  zmq::message_t _handle_client_lookup(central::client_id const& id);

public:
  central_server() = delete;
  central_server(std::string const& addr);

  void run();
};

class central_client {
 private:
  zmq::context_t _context;
  zmq::socket_t _socket;

 public:
  central_client() = delete;
  central_client(std::string const& addr);

  bool client_register(std::string const& name, uint32_t port);
  bool client_lookup(std::string const& name, std::string& addr);
};
#endif // P2P_CLIENT_CENTRAL_IFACE_H_
