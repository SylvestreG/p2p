//
// Created by syl on 1/22/20.
//

#ifndef P2P_CLIENT_CENTRAL_IFACE_H_
#define P2P_CLIENT_CENTRAL_IFACE_H_

#include <zmq.h>
#include "central.pb.h"

class central_client {
 private:
  void *_context;
  void *_socket;

 public:
  central_client() = delete;
  central_client(std::string const& addr);
  ~central_client();

  bool client_register(std::string const& name, uint32_t port);
  bool client_lookup(std::string const& name, std::string& addr);
  void stop_server(void);
};

#endif //P2P_CLIENT_CENTRAL_IFACE_H_