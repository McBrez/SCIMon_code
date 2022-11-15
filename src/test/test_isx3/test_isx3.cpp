// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

// Project includes
#include <device_isx3.hpp>
#include <init_message_isx3.hpp>

using namespace Devices;

TEST_CASE("Testing the implementation of the Sciospec ISX3 device",
          "[Devices]") {

  SECTION("Initializing") {
    DeviceIsx3 dut;
    REQUIRE(dut.write(shared_ptr<InitMessageIsx3>()) == true);
  }
}
