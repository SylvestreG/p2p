cmake_minimum_required(VERSION 3.10)
project(p2p)

set(CMAKE_CXX_STANDARD 17)

include(ExternalProject)

ExternalProject_Add(json11_lib
  GIT_REPOSITORY https://github.com/dropbox/json11/
  TIMEOUT 10
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON)

ExternalProject_Get_Property(json11_lib source_dir)
ExternalProject_Get_Property(json11_lib binary_dir)

link_directories(${binary_dir})
include_directories(${source_dir})

# Check is the library is installed on the system
find_library(ZEROMQ_LIBRARIES NAMES zmq DOC "Path to libzmq)")

find_path(ZEROMQ_INCLUDE_DIR NAMES zmq.h DOC "Path to ZeroMQ include header files.")

include_directories(${ZEROMQ_INCLUDE_DIR})
link_directories(${ZEROMQ_LIBRARIES})

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/proto/central.proto)

add_executable(p2p-server src/central/server.cpp src/central/central.cpp ${PROTO_SRCS} ${PROTO_HDRS})
add_dependencies(p2p-server json11_lib)
target_link_libraries(p2p-server json11 ${Protobuf_LIBRARIES} zmq)

add_executable(p2p-client src/client/client.cpp src/client/central_client.cpp ${PROTO_SRCS} ${PROTO_HDRS})
add_dependencies(p2p-client json11_lib)
target_link_libraries(p2p-client json11 ${Protobuf_LIBRARIES} zmq)


option(WITH_TESTING "Generate unit tests." OFF)
add_subdirectory(tests)