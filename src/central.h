//
// Created by syl on 1/22/20.
//

#ifndef P2P_CLIENT_CENTRAL_IFACE_H_
#define P2P_CLIENT_CENTRAL_IFACE_H_

#include <iostream>
#include <memory>
#include <string>
#include <zmq.hpp>
#include "comm.pb.h"

class central_server {
private:
  zmq::context_t _context;
  zmq::socket_t _socket;

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
  void send(std::string const& msg);
};
#endif // P2P_CLIENT_CENTRAL_IFACE_H_
