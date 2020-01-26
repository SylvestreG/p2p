ExternalProject_Add(googletest
  GIT_REPOSITORY https://github.com/google/googletest/
  TIMEOUT 10
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON)
ExternalProject_Get_Property(googletest source_dir)
ExternalProject_Get_Property(googletest binary_dir)
include_directories(${source_dir}/googletest/include/)
link_directories(${binary_dir}/lib)

add_definitions(-DTEST_DIR="${CMAKE_SOURCE_DIR}/tests/")

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/proto/central.proto)

add_executable(ut main.cpp central.cpp ../src/client/central_client.cpp ../src/central/central.cpp ${PROTO_SRCS})
add_dependencies(ut googletest p2p-server)
target_link_libraries(ut gtest pthread zmq ${Protobuf_LIBRARIES})
