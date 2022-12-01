// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <device_ob1.hpp>

using namespace Devices;
using namespace Messages;

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing the implementation of the ElveFlow OB1 device",
          "[Devices]") {
  // Build up
  DeviceOb1 *dut = new DeviceOb1();

  SECTION("Initializing") {

    REQUIRE(dut->write(shared_ptr<InitDeviceMessage>()) == true);
    auto ackMsg = dut->read();
    REQUIRE(ackMsg);
    REQUIRE(ackMsg->getData() == "ACK");
  }

  // Tear down.
  delete dut;
}
