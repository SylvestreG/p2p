P2P
===

[![Build Status](https://travis-ci.org/SylvestreG/p2p.png)](https://travis-ci.org/SylvestreG/p2p)

## Overview

p2p is a cli full c++ mesasging proxy

## Installation

### Requirements
* [Required] - [zmq]
* [Required] - [protobuf]

Installing dependencies on MacOsX, you need brew

```bash
$ brew install zmq 
$ brew install protobuf
```

For Linux you only need boost

for ArchLinux
```bash
$ pacman -S zeromq protobuf
```

for Ubuntu
```bash
apt-get install libzmq3-dev protobuf-compiler libprotobuf-dev
```
### Building

```bash
$ git clone https://github.com/SylvestreG/p2p.git
$ cd sdk && mkdir build && cd build
$ cmake .. && make
```

## How to use
launch a central.
launch p2p client to communicate.

### Testing

in a terminal launch:
```bash
$ ./central -p 4242
```
in another terminal launch a client a
```bash
$ ./p2p -n a -p 4243 -c tcp://127.0.0.1:4242
```

in another terminal launch a client a
```bash
$ ./p2p -n b -p 4243 -c tcp://127.0.0.1:4243
p2p> send a coucou
```

A should receive the msg. you can type help if you want a list of possible 
commands...
