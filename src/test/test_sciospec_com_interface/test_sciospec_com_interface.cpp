// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <com_interface_codec.hpp>
#include <is_payload.hpp>
#include <isx3_command_buffer.hpp>

INITIALIZE_EASYLOGGINGPP

using namespace std::complex_literals;

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

TEST_CASE("Testing ComInterfaceCodec::decodeMessage()") {
  Devices::ComInterfaceCodec dut;
  SECTION("Decode Impedance Data") {
    // Encodes a impedance measurement result with the following content:
    // [f_number] = 1
    // [time stamp] = 	1677711616.0 (2. March. 2023 00:16 GMT)
    // [channel number] = 1
    // [real] = 150.0
    // [imag] = 200.0
    std::vector<unsigned char> frame({0xB8, 0x10, 0x00, 0x01, 0x4e, 0xC7, 0xFF,
                                      0xB2, 0x00, 0x01, 0x43, 0x16, 0x00, 0x00,
                                      0x43, 0x48, 0x00, 0x00, 0xB8});

    shared_ptr<Devices::ReadPayload> result = dut.decodeMessage(frame);

    // Result pointer should not be null.
    REQUIRE(result);
    shared_ptr<Devices::IsPayload> resultCasted =
        dynamic_pointer_cast<Devices::IsPayload>(result);
    // Result should be castable to IsPayload.
    REQUIRE(resultCasted);
    // Result should have the correct data.
    REQUIRE(resultCasted->getChannelNumber() == 1);
    REQUIRE((float)resultCasted->getTimestamp() == 1677711600.0f);
    // There should be exactly one ImpedancePoint entry.
    REQUIRE(resultCasted->getImpedanceSpectrum().size() == 1);
    ImpedancePoint impedancePoint =
        resultCasted->getImpedanceSpectrum().front();
    double frequency = std::get<0>(impedancePoint);
    complex<double> impedance = std::get<1>(impedancePoint);

    REQUIRE(frequency == 1.0);
    REQUIRE(impedance == (150.0 + 200.0i));
  }
}

TEST_CASE("Testing Isx3CommandBuffer") {
  Devices::Isx3CommandBuffer dut;
  SECTION("Push valid command at once") {
    std::vector<unsigned char> testBytes(
        {0xB8, 0x10, 0x00, 0x01, 0x4e, 0xc7, 0xff, 0xce, 0x00, 0x01, 0x43, 0x16,
         0x00, 0x00, 0x43, 0x48, 0x00, 0x00, 0xB8});
    dut.pushBytes(std::vector<unsigned char>(testBytes));
    std::vector<unsigned char> resultByte = dut.interpretBuffer();
    REQUIRE(resultByte == testBytes);
  }

  SECTION("Push valid command at two different occasions") {
    std::vector<unsigned char> testBytes1(
        {0xB8, 0x10, 0x00, 0x01, 0x4e, 0xc7, 0xff, 0xce});
    std::vector<unsigned char> testBytes2(
        {0x00, 0x01, 0x43, 0x16, 0x00, 0x00, 0x43, 0x48, 0x00, 0x00, 0xB8});
    std::vector<unsigned char> referenceBytes(
        {0xB8, 0x10, 0x00, 0x01, 0x4e, 0xc7, 0xff, 0xce, 0x00, 0x01, 0x43, 0x16,
         0x00, 0x00, 0x43, 0x48, 0x00, 0x00, 0xB8});

    dut.pushBytes(std::vector<unsigned char>(testBytes1));
    std::vector<unsigned char> resultBytes1 = dut.interpretBuffer();
    REQUIRE(resultBytes1 == std::vector<unsigned char>());
    dut.pushBytes(std::vector<unsigned char>(testBytes2));
    std::vector<unsigned char> resultBytes2 = dut.interpretBuffer();
    REQUIRE(resultBytes2 == referenceBytes);
  }

  SECTION("Push valid command at once with padded bytes") {

    std::vector<unsigned char> testBytes(
        {0x00, 0x00, 0x00,

         0xB8, 0x10, 0x00, 0x01, 0x4e, 0xc7, 0xff, 0xce, 0x00, 0x01,
         0x43, 0x16, 0x00, 0x00, 0x43, 0x48, 0x00, 0x00, 0xB8,

         0x00, 0x00, 0x00});
    std::vector<unsigned char> referenceBytes(
        {0xB8, 0x10, 0x00, 0x01, 0x4e, 0xc7, 0xff, 0xce, 0x00, 0x01, 0x43, 0x16,
         0x00, 0x00, 0x43, 0x48, 0x00, 0x00, 0xB8});

    dut.pushBytes(testBytes);
    std::vector<unsigned char> resultBytes = dut.interpretBuffer();
    REQUIRE(resultBytes == referenceBytes);
  }

  SECTION("Push valid command at two different occasions with padded bytes") {
    std::vector<unsigned char> testBytes1({0x00, 0x00, 0x00,

                                           0xB8, 0x10, 0x00, 0x01, 0x4e, 0xc7,
                                           0xff, 0xce});
    std::vector<unsigned char> testBytes2({0x00, 0x01, 0x43, 0x16, 0x00, 0x00,
                                           0x43, 0x48, 0x00, 0x00, 0xB8,

                                           0x00, 0x00, 0x00});
    std::vector<unsigned char> referenceBytes(
        {0xB8, 0x10, 0x00, 0x01, 0x4e, 0xc7, 0xff, 0xce, 0x00, 0x01, 0x43, 0x16,
         0x00, 0x00, 0x43, 0x48, 0x00, 0x00, 0xB8});

    dut.pushBytes(std::vector<unsigned char>(testBytes1));
    std::vector<unsigned char> resultBytes1 = dut.interpretBuffer();
    REQUIRE(resultBytes1 == std::vector<unsigned char>());
    dut.pushBytes(std::vector<unsigned char>(testBytes2));
    std::vector<unsigned char> resultBytes2 = dut.interpretBuffer();
    REQUIRE(resultBytes2 == referenceBytes);
  }
}
