// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <config_is_message.hpp>
#include <device_ob1.hpp>

using namespace Devices;

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing the implementation of the ElveFlow OB1 device",
          "[Devices]") {

  SECTION("Initializing") {
    DeviceOb1 &dut = DeviceOb1();
    REQUIRE(dut.write(shared_ptr<InitDeviceMessage>()) == true);
    auto ackMsg = dut.read();
    REQUIRE(ackMsg);
    REQUIRE(ackMsg->getData() == "ACK");
  }
}
