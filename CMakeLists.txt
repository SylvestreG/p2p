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

include(FindPkgConfig)
pkg_check_modules(PROTOBUF REQUIRED protobuf)
pkg_check_modules(GRPC REQUIRED grpc++)
set(GRPC_CPP_PLUGIN "${GRPC_PREFIX}/bin/grpc_cpp_plugin")

add_custom_command(
  DEPENDS ${CMAKE_SOURCE_DIR}/proto/comm.proto
  COMMENT "Generating interface files of the proto file (grpc)"
  OUTPUT ${CMAKE_SOURCE_DIR}/src/comm.grpc.pb.cc ${CMAKE_SOURCE_DIR}/src/comm.grpc.pb.h
  COMMAND ${PROTOBUF_PREFIX}/bin/protoc
  ARGS --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} --proto_path=${CMAKE_SOURCE_DIR}/proto --grpc_out="${CMAKE_SOURCE_DIR}/src/" ${CMAKE_SOURCE_DIR}/proto/comm.proto

  DEPENDS ${CMAKE_SOURCE_DIR}/proto/comm.proto
  COMMENT "Generating interface files of the proto file (protobuf)"
  OUTPUT ${CMAKE_SOURCE_DIR}/src/comm.pb.cc ${CMAKE_SOURCE_DIR}/comm.pb.h
  COMMAND ${PROTOBUF_PREFIX}/bin/protoc
  ARGS --cpp_out="${CMAKE_SOURCE_DIR}/src" --proto_path=${CMAKE_SOURCE_DIR}/proto ${CMAKE_SOURCE_DIR}/proto/comm.proto
)

add_library(rpc ${CMAKE_SOURCE_DIR}/src/comm.pb.cc ${CMAKE_SOURCE_DIR}/src/comm.grpc.pb.cc)

add_executable(p2p-server src/server.cpp src/central_server.cpp)
add_dependencies(p2p-server json11_lib)
target_link_libraries(p2p-server json11 ${GRPC_STATIC_LDFLAGS} rpc)

add_executable(p2p-client src/client.cpp src/central_server.cpp)
add_dependencies(p2p-client json11_lib)
target_link_libraries(p2p-client json11 ${GRPC_STATIC_LDFLAGS} rpc)