// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

#include <com_interface_codec.hpp>

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing ComInterfaceCodec::buildCmdGetDeviceId()") {
  const std::vector<unsigned char> getDeviceIdCmdReference({0xD1, 0x00, 0xD1});
  Devices::ComInterfaceCodec dut;
  std::vector<unsigned char> getDeviceIdCmd = dut.buildCmdGetDeviceId();

  REQUIRE(getDeviceIdCmdReference == getDeviceIdCmd);
}

TEST_CASE("Testing ComInterfaceCodec::buildCmdSetSetup()") {
  Devices::ComInterfaceCodec dut;
  SECTION("Init Setup") {
    const std::vector<unsigned char> SetSetupInitReference(
        {0xB6, 0x01, 0x01, 0xB6});

    std::vector<unsigned char> setSetupInitCmd = dut.buildCmdSetSetup();

    REQUIRE(SetSetupInitReference == setSetupInitCmd);
  }

  SECTION("Frequency Point") {
    // This command should configure a frequency point at 10Hz with a precision
    // of 1.0 and an amplitude of 0.5.
    const std::vector<unsigned char> setSetupFrequencyPointReference(
        {0xB6, 0x0D, 0x02, 0x41, 0x20, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x3f,
         0x00, 0x00, 0x00, 0xB6});

    std::vector<unsigned char> setSetupFrequencyPointCmd =
        dut.buildCmdSetSetup(10.0, 1, 0.5);

    REQUIRE(setSetupFrequencyPointReference == setSetupFrequencyPointCmd);
  }

  SECTION("Frequency List") {
    // This command should configure a frequency list with following parameters:
    // [f_start] = 10.0 Hz
    // [f_stop] = 100.0 Hz
    // [f_count] = 50
    // [f_scale] = Linear
    // [precision] 1.0
    // [amplitude] = 0.5
    const std::vector<unsigned char> setSetupFrequencyListReference(
        {0xB6, 0x16, 0x03, 0x41, 0x20, 0x00, 0x00, 0x42, 0xc8,
         0x00, 0x00, 0x42, 0x48, 0x00, 0x00, 0x00, 0x3f, 0x80,
         0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0xB6});

    std::vector<unsigned char> SetSetupFrequencyListCmd = dut.buildCmdSetSetup(
        10.0, 100.0, 50, Devices::FrequencyScale::FREQ_SCALE_LINEAR, 1.0, 0.5);

    REQUIRE(setSetupFrequencyListReference == SetSetupFrequencyListCmd);
  }
}
