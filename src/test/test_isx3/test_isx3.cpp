#undef _HAS_STD_BYTE

// Standard includes
#include <thread>

// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <config_device_message.hpp>
#include <device_isx3_win.hpp>
#include <init_message_isx3.hpp>
#include <is_configuration.hpp>
#include <isx3_software_mocker.hpp>

using namespace Devices;

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing the implementation of the Sciospec ISX3 device",
          "[Devices]") {

  // Uncomment, if ISX3 Software shall be mocked.
  // #define MOCK_ISX3_SOFTWARE

  SECTION("Initializing") {
    int telnetPort = 2811;
#ifdef MOCK_ISX3_SOFTWARE
    // Create the ISX3 sofware mocker.
    Isx3SoftwareMocker softwareMocker(telnetPort);
    softwareMocker.run();
#endif
    // Create the DUT.
    DeviceIsx3 *dut = new DeviceIsx3Win();
    // Create an init message.
    shared_ptr<InitDeviceMessage> initMsg(
        new InitMessageIsx3("127.0.0.1", telnetPort));
    REQUIRE(dut->write(initMsg) == true);

    // After an init message, an empty response is expected.
    shared_ptr<DeviceMessage> response = dut->read();
    REQUIRE(!response);

    // Configure the dut.
    DeviceConfiguration *deviceConfig = new IsConfiguration(
        10.0, 100.0, 3, 0, "BNC", IsScale::LINEAR_SCALE, 0.0, 250.0, 0, 0);
    shared_ptr<ConfigDeviceMessage> configMsg(
        new ConfigDeviceMessage(deviceConfig));
    REQUIRE(dut->write(configMsg) == true);

    // After an configure message, an empty response is expected.
    response = dut->read();
    REQUIRE(!response);

    // Start measuring.
    shared_ptr<WriteDeviceMessage> startMsg(
        new WriteDeviceMessage(WriteDeviceTopic::RUN_TOPIC));
    REQUIRE(dut->write(startMsg) == true);

    // Do some reads.
    for (int i = 0; i < 10; i++) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      auto readMsg = dut->read();

      if (readMsg) {
        LOG(INFO) << readMsg->serialize();
      }
    }

#ifdef MOCK_ISX3_SOFTWARE
    softwareMocker.stop();
#endif
    // Delete the dut again.
    delete dut;
  }
}
