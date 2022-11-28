// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <config_is_message.hpp>
#include <device_isx3_win.hpp>
#include <init_message_isx3.hpp>
#include <isx3_software_mocker.hpp>

using namespace Devices;

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing the implementation of the Sciospec ISX3 device",
          "[Devices]") {

  SECTION("Initializing") {
    // Create the ISX3 sofware mocker.
    int telnetPort = 23;
    Isx3SoftwareMocker softwareMocker(telnetPort);
    softwareMocker.run();

    // Create the DUT.
    DeviceIsx3 *dut = new DeviceIsx3Win();
    // Create an init message.
    shared_ptr<InitDeviceMessage> initMsg(
        new InitMessageIsx3("127.0.0.1", telnetPort));
    REQUIRE(dut->write(initMsg) == true);

    // After an init message, an empty response is expected.
    shared_ptr<DeviceMessage> response = dut->read();
    REQUIRE(!response);

    softwareMocker.stop();
    // Delete the dut again.
    delete dut;
  }

  SECTION("Configure a setup") {
    // Create the ISX3 sofware mocker.
    int telnetPort = 23;
    Isx3SoftwareMocker softwareMocker(telnetPort);
    softwareMocker.run();

    // Create the DUT.
    DeviceIsx3 *dut = new DeviceIsx3Win();
    // Create an init message.
    shared_ptr<InitDeviceMessage> initMsg(
        new InitMessageIsx3("127.0.0.1", telnetPort));
    REQUIRE(dut->write(initMsg) == true);

    // After an init message, an empty response is expected.
    shared_ptr<DeviceMessage> response = dut->read();
    REQUIRE(!response);

    // Delete the dut again.
    delete dut;
  }
}
