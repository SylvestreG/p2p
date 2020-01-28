//
// Created by syl on 1/28/20.
//

#ifndef P2P_SRC_ZMQ_HELPER_H_
#define P2P_SRC_ZMQ_HELPER_H_

#include <string>

class zmq_helper {
public:
  typedef enum  {
    requester,
    replier
  }zmq_socket_type;

private:
  void *_context;
  void *_socket;

public:
  zmq_helper(std::string const& addr, zmq_helper::zmq_socket_type type);
  ~zmq_helper();

  void send(std::string const& data);
  void recv(std::string & data);
  void recv_and_get_peer(std::string & data, std::string &peer);
};

#endif // P2P_SRC_ZMQ_HELPER_H_
