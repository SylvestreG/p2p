//
// Created by syl on 1/22/20.
//

#include "central.h"

central_client::central_client(std::string const &addr)
    : _context{1}, _socket{_context, ZMQ_REQ} {
  _socket.connect("tcp://localhost:5555");
}

void central_client::send(std::string const &msg) {
  zmq::message_t request(msg.size());
  memcpy(request.data(), msg.c_str(), msg.size());
  _socket.send(request);

  //  Get the reply.
  zmq::message_t reply;
  _socket.recv(&reply);
}

central_server::central_server(std::string const &addr)
    : _context{1}, _socket{_context, ZMQ_REP} {
  //  Prepare our context and socket
  _socket.bind("tcp://*:5555");
}

void central_server::run() {
  while (true) {
    zmq::message_t request;

    //  Wait for next request from client
    _socket.recv(&request);
    std::cout << "Received Hello" << std::endl;;

    //  Send reply back to client
    zmq::message_t reply(5);
    memcpy(reply.data(), "World", 5);
    _socket.send(reply);
  }
}
