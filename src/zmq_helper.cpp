//
// Created by syl on 1/28/20.
//

#include "zmq_helper.h"
#include <cstring>
#include <zmq.h>

zmq_helper::zmq_helper(std::string const &addr,
                       zmq_helper::zmq_socket_type type) {
  _context = zmq_ctx_new();
  _socket = zmq_socket(_context, (type == replier) ? ZMQ_REP : ZMQ_REQ);

  if (type == replier)
    zmq_bind(_socket, addr.c_str());
  else
    zmq_connect(_socket, addr.c_str());
}

zmq_helper::~zmq_helper() {
  zmq_close(_socket);
  zmq_ctx_destroy(_context);
}

void
zmq_helper::send(std::string const& data) {
  zmq_msg_t msg;
  zmq_msg_init_size(&msg, data.length());
  memcpy(zmq_msg_data(&msg), data.c_str(), data.length());
  zmq_msg_send(&msg, _socket, 0);
  zmq_msg_close(&msg);
}

void
zmq_helper::recv(std::string & data) {
  zmq_msg_t msg;
  zmq_msg_init(&msg);
  zmq_msg_recv(&msg, _socket, 0);
  data = std::string(static_cast<char *>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
  zmq_msg_close(&msg);
}

void
zmq_helper::recv_and_get_peer(std::string & data, std::string &peer) {
  zmq_msg_t msg;
  zmq_msg_init(&msg);
  zmq_msg_recv(&msg, _socket, 0);
  data = std::string(static_cast<char *>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
  peer = zmq_msg_gets(&msg, "Peer-Address");
  zmq_msg_close(&msg);
}