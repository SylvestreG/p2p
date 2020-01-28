//
// Created by syl on 1/22/20.
//

#ifndef P2P_CENTRAL_IFACE_H_
#define P2P_CENTRAL_IFACE_H_

#include "../zmq_helper.h"
#include "central.pb.h"
#include <string>
#include <unordered_map>

class central_server {
private:
  zmq_helper _zmq;

  std::unordered_map<std::string, std::string> _client_map; // name -> addr

  std::string _handle_client_register(central::client_information const &infos,
                                      std::string const &peer);
  std::string _handle_client_lookup(central::client_id const &id);
  std::string _handle_client_unregister(central::client_id const &id);

public:
  central_server() = delete;
  ~central_server();
  central_server(std::string const &addr);

  void run();
};

#endif // P2P_CENTRAL_IFACE_H_
