//
// Created by syl on 1/26/20.
//

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

std::shared_ptr<spdlog::logger> logger;

int main(int argc, char *argv[]) {
  // GTest initialization.
  testing::InitGoogleTest(&argc, argv);

  logger = spdlog::stdout_color_mt("global");
  // Run all tests.
  return (RUN_ALL_TESTS());
}