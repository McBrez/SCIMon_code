// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <message_distributor.hpp>

INITIALIZE_EASYLOGGINGPP

using namespace Messages;

TEST_CASE("") {
  MessageDistributor dut;
  dut.subscribe();
}
