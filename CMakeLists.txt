cmake_minimum_required(VERSION 3.10)
project(p2p)

set(CMAKE_CXX_STANDARD 17)

include(ExternalProject)

ExternalProject_Add(spdlog_lib
  GIT_REPOSITORY https://github.com/gabime/spdlog/
  TIMEOUT 10
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON)
add_definitions("-DSPDLOG_ENABLE_SYSLOG")
ExternalProject_Get_Property(spdlog_lib source_dir)
include_directories(${source_dir}/include)

ExternalProject_Add(lyra-lib
  GIT_REPOSITORY https://github.com/bfgroup/lyra
  TIMEOUT 10
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON)
ExternalProject_Get_Property(lyra-lib source_dir)
include_directories(${source_dir}/include/)

ExternalProject_Add(replxx_lib
  GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx
  TIMEOUT 10
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON)

ExternalProject_Get_Property(replxx_lib source_dir)
ExternalProject_Get_Property(replxx_lib binary_dir)


link_directories(${binary_dir})
include_directories(${source_dir}/include)

# Check is the library is installed on the system
find_library(ZEROMQ_LIBRARIES NAMES zmq DOC "Path to libzmq)")

find_path(ZEROMQ_INCLUDE_DIR NAMES zmq.h DOC "Path to ZeroMQ include header files.")

include_directories(${ZEROMQ_INCLUDE_DIR})
link_directories(${ZEROMQ_LIBRARIES})

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(CENTRAL_SRCS CENTRAL_HDRS ${CMAKE_SOURCE_DIR}/proto/central.proto)
protobuf_generate_cpp(P2P_SRCS P2P_HDRS ${CMAKE_SOURCE_DIR}/proto/p2p.proto)

add_executable(central  src/central/central.cpp
                        src/central/main.cpp
                        src/zmq_helper.cpp
                        ${CENTRAL_SRCS} ${CENTRAL_HDRS})
add_dependencies(central lyra-lib spdlog_lib)
target_link_libraries(central ${Protobuf_LIBRARIES} zmq)

add_executable(p2p      src/client/main.cpp
                        src/client/central_client.cpp
                        src/client/cli.cpp
                        src/client/cli_cmd.cpp
                        src/client/utils.cpp
                        src/zmq_helper.cpp
                        ${CENTRAL_SRCS}
                        ${CENTRAL_HDRS}
                        ${P2P_SRCS}
                        ${P2O_HDRS})
add_dependencies(p2p replxx_lib lyra-lib spdlog_lib)
target_link_libraries(p2p replxx ${Protobuf_LIBRARIES} zmq)

option(WITH_TESTING "Generate unit tests." OFF)
add_subdirectory(tests)