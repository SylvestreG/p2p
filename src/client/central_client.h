//
// Created by syl on 1/22/20.
//

#ifndef P2P_CLIENT_CENTRAL_IFACE_H_
#define P2P_CLIENT_CENTRAL_IFACE_H_

#include <zmq.hpp>
#include "central.pb.h"

class central_client {
 private:
  zmq::context_t _context;
  zmq::socket_t _socket;

 public:
  central_client() = delete;
  central_client(std::string const& addr);

  bool client_register(std::string const& name, uint32_t port);
  bool client_lookup(std::string const& name, std::string& addr);
  bool stop_server(void);
};

#endif //P2P_CLIENT_CENTRAL_IFACE_H_