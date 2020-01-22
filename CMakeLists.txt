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

find_package(Protobuf REQUIRED)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(GRPC REQUIRED)


set(PROTOS ${CMAKE_SOURCE_DIR}/proto/comm.proto)
set(PROTO_SRC_DIR ${CMAKE_BINARY_DIR}/proto)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
include_directories(${PROTO_SRC_DIR})
include_directories(${CMAKE_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS})

add_executable(p2p-client client.cpp client/central_server.cpp  ${PROTO_SRCS}  ${GRPC_SRCS})
add_dependencies(p2p-client json11_lib)
target_link_libraries(p2p-client json11 gRPC::grpc++_reflection protobuf::libprotobuf)