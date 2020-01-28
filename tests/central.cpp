//
// Created by syl on 1/26/20.
//

#include "../src/central/central.h"
#include "../src/client/central_client.h"
#include <gtest/gtest.h>
#include <thread>

const std::string server_addr{"tcp://127.0.0.1:4343"};

class central_test : public ::testing::Test {
private:
  void SetUp() override {
    _server_thread = std::thread(&central_server::run, &_server);
  }

  void TearDown() override {
    central_client cli{server_addr};
    cli.stop_server();

    _server_thread.join();
  }

  std::thread _server_thread;
  central_server _server;

public:
  central_test() : _server{"tcp://*:4343"} {}
};

TEST_F(central_test, simple) {}

TEST_F(central_test, reg) {
  central_client cli1{server_addr};
  central_client cli2{server_addr};
  central_client cli3{server_addr};

  ASSERT_TRUE(cli1.client_register("1", 3232));
  ASSERT_TRUE(cli1.client_register("1", 3232));
  ASSERT_FALSE(cli1.client_register("1", 3233));

  ASSERT_TRUE(cli2.client_register("2", 3233));
  ASSERT_TRUE(cli3.client_register("3", 3234));
}

TEST_F(central_test, lookup) {
  central_client cli1{server_addr};
  central_client cli2{server_addr};
  central_client cli3{server_addr};

  ASSERT_TRUE(cli1.client_register("1", 3232));
  ASSERT_TRUE(cli2.client_register("2", 3233));
  ASSERT_TRUE(cli3.client_register("3", 3234));

  std::string output;
  ASSERT_FALSE(cli1.client_lookup("4", output));
  ASSERT_TRUE(cli1.client_lookup("1", output));
  ASSERT_EQ(output, "tcp://127.0.0.1:3232");
  ASSERT_TRUE(cli1.client_lookup("2", output));
  ASSERT_EQ(output, "tcp://127.0.0.1:3233");
  ASSERT_TRUE(cli1.client_lookup("3", output));
  ASSERT_EQ(output, "tcp://127.0.0.1:3234");

  ASSERT_FALSE(cli2.client_lookup("4", output));
  ASSERT_TRUE(cli2.client_lookup("1", output));
  ASSERT_EQ(output, "tcp://127.0.0.1:3232");
  ASSERT_TRUE(cli2.client_lookup("2", output));
  ASSERT_EQ(output, "tcp://127.0.0.1:3233");
  ASSERT_TRUE(cli2.client_lookup("3", output));
  ASSERT_EQ(output, "tcp://127.0.0.1:3234");

  ASSERT_FALSE(cli3.client_lookup("4", output));
  ASSERT_TRUE(cli3.client_lookup("1", output));
  ASSERT_EQ(output, "tcp://127.0.0.1:3232");
  ASSERT_TRUE(cli3.client_lookup("2", output));
  ASSERT_EQ(output, "tcp://127.0.0.1:3233");
  ASSERT_TRUE(cli3.client_lookup("3", output));
  ASSERT_EQ(output, "tcp://127.0.0.1:3234");
}
