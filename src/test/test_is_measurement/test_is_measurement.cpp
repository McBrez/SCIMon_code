// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <main_control.hpp>

using namespace Control;
using namespace chrono_literals;

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing the implementation of the main control loop") {
  // Build up
  Utilities::Duration interval = chrono::milliseconds(10ms);
  MainControl *dut = new MainControl(interval);

  SECTION("Initializing") { dut->start(); }
}
