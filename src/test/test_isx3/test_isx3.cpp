// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <config_is_message.hpp>
#include <device_isx3_win.hpp>
#include <init_message_isx3.hpp>

using namespace Devices;

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing the implementation of the Sciospec ISX3 device",
          "[Devices]") {

  SECTION("Initializing") {
    DeviceIsx3 &dut = DeviceIsx3Win();
    REQUIRE(dut.write(shared_ptr<InitMessageIsx3>()) == true);
    auto ackMsg = dut.read();
    REQUIRE(ackMsg);
    REQUIRE(ackMsg->getData() == "ACK");
  }

  SECTION("Configure a setup") {
    DeviceIsx3 &dut = DeviceIsx3Win();
    REQUIRE(dut.write(shared_ptr<InitMessageIsx3>()) == true);

    shared_ptr<ConfigIsMessage> configMessage(new ConfigIsMessage(
        10.0, 100.0, 10, IsScale::LINEAR, 1.0, 0.25, false));
    REQUIRE(dut.write(configMessage) == true);
    auto ackMsg = dut.read();
    REQUIRE(ackMsg->getData() == "ACK");
  }
}
