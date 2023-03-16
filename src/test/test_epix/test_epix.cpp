// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <device_isx3.hpp>
#include <id_payload.hpp>
#include <isx3_ack_payload.hpp>
#include <win_socket.hpp>

using namespace Devices;

INITIALIZE_EASYLOGGINGPP

/**
 * @brief Reads from the socket wrapper until no more bytes are received.
 * @param socketWrapper Pointer to the socket wrapper.
 * @return The bytes that have been read.
 */
std::vector<unsigned char>
readFromSocket(shared_ptr<SocketWrapper> socketWrapper) {
  // Read until device no longer sends data.
  std::vector<unsigned char> readBuffer;
  int bytesRead = -1;
  while (bytesRead > 0) {
    std::vector<unsigned char> tempBuffer;
    bytesRead = socketWrapper->read(tempBuffer);
    for (auto byteValue : tempBuffer) {
      readBuffer.push_back(byteValue);
    }
  }

  return readBuffer;
}

std::list<std::vector<unsigned char>> extractFrames(Isx3CommandBuffer &buffer) {
  std::list<std::vector<unsigned char>> frameBuffer;
  bool frameDetected = false;
  do {
    std::vector<unsigned char> frame = buffer.interpretBuffer();

    if (frame.empty()) {
      frameDetected = false;
    } else {
      frameDetected = true;
      frameBuffer.push_back(frame);
    }
  } while (frameDetected);

  return frameBuffer;
}

TEST_CASE("Test the backend communication classes") {
  shared_ptr<Utilities::SocketWrapper> socketWrapper(
      new Utilities::WinSocket());
  Devices::ComInterfaceCodec codec;
  Devices::Isx3CommandBuffer buffer;

  SECTION("Connect and disconnect from device") {
    // Connect to the socket.
    bool connectionSuccess = socketWrapper->open("127.0.0.1", 8888);
    REQUIRE(connectionSuccess);
    // And disconnect again.
    bool disconnectSuccess = socketWrapper->close();
    REQUIRE(disconnectSuccess);
  }
  SECTION("Try to get device id") {
    // Connect to the socket.
    bool connectionSuccess = socketWrapper->open("127.0.0.1", 8888);
    REQUIRE(connectionSuccess);

    // Send the GetDeviceId command to device.
    std::vector<unsigned char> getDeviceIdCmd = codec.buildCmdGetDeviceId();
    socketWrapper->write(getDeviceIdCmd);

    // Read until device no longer sends data.
    std::vector<unsigned char> readBuffer = readFromSocket(socketWrapper);

    // Push the received bytes into the command buffer and try to extract the
    // received frames.
    buffer.pushBytes(readBuffer);
    std::list<std::vector<unsigned char>> frameBuffer = extractFrames(buffer);

    // Decode the frames into payloads. The first frame should be the device id.
    // The second one should be a positive acknowledge.
    // There should be exactly to frames.
    REQUIRE(frameBuffer.size() == 2);
    auto frameBufferIt = frameBuffer.begin();
    std::shared_ptr<Devices::ReadPayload> deviceIdPayload =
        codec.decodeMessage(*frameBufferIt);
    // Device id response.
    // Payload should be not null.
    REQUIRE(deviceIdPayload);
    // Payload should be an id payload.
    std::shared_ptr<Devices::IdPayload> deviceIdPayloadCasted =
        dynamic_pointer_cast<Devices::IdPayload>(deviceIdPayload);
    REQUIRE(deviceIdPayloadCasted);
    // Acknowledge
    ++frameBufferIt;
    std::shared_ptr<Devices::ReadPayload> ackPayload =
        codec.decodeMessage(*frameBufferIt);
    // Payload should be not null.
    REQUIRE(ackPayload);
    // Payload should be an ack payload.
    std::shared_ptr<Devices::Isx3AckPayload> ackPayloadCasted =
        dynamic_pointer_cast<Devices::Isx3AckPayload>(ackPayload);
    REQUIRE(ackPayloadCasted);
    // Acknowledgment has to be positive.
    REQUIRE(ackPayloadCasted->getAckType() ==
            Isx3AckType::ISX3_ACK_TYPE_COMMAND_ACKNOWLEDGE);

    // Disconnect from the socket.
    bool disconnectSuccess = socketWrapper->close();
    REQUIRE(disconnectSuccess);
  }

  SECTION("Try to set up a measurement") {
    bool connectionSuccess = socketWrapper->open("127.0.0.1", 8888);
    REQUIRE(connectionSuccess);
    std::vector<unsigned char> readBuffer;

    // Remove any previously configured setup.
    auto initSetupCmd = codec.buildCmdSetSetup();
    socketWrapper->write(initSetupCmd);
    // Check if acknowledgment has been received.
    readBuffer = readFromSocket(socketWrapper);
    buffer.pushBytes(readBuffer);
    auto initFrames = extractFrames(buffer);
    // There should be exactly one frame.
    REQUIRE(initFrames.size() == 1);
    // Decode the frame and check if it is an positive acknowledgment.
    auto initAckPayload = codec.decodeMessage(initFrames.front());
    REQUIRE(initAckPayload);
    auto initAckPayloadCasted =
        dynamic_pointer_cast<Devices::Isx3AckPayload>(initAckPayload);
    REQUIRE(initAckPayloadCasted);
    REQUIRE(initAckPayloadCasted->getAckType() ==
            Isx3AckType::ISX3_ACK_TYPE_COMMAND_ACKNOWLEDGE);
    readBuffer.clear();

    // Add a list of frequencies.
    auto setFreqListCmd = codec.buildCmdSetSetup(
        10.0, 100.0, 15.0, Devices::FrequencyScale::FREQ_SCALE_LINEAR, 1.0,
        0.25);
    socketWrapper->write(setFreqListCmd);
    // Check if acknowledgment has been received.
    readBuffer = readFromSocket(socketWrapper);
    buffer.pushBytes(readBuffer);
    auto setFreqListFrames = extractFrames(buffer);
    // There should be exactly one frame.
    REQUIRE(setFreqListFrames.size() == 1);
    // Decode the frame and check if it is an positive acknowledgment.
    auto setFreqListAckPayload = codec.decodeMessage(setFreqListFrames.front());
    REQUIRE(setFreqListAckPayload);
    auto setFrequListPayloadCasted =
        dynamic_pointer_cast<Devices::Isx3AckPayload>(setFreqListAckPayload);
    REQUIRE(setFrequListPayloadCasted);
    REQUIRE(setFrequListPayloadCasted->getAckType() ==
            Isx3AckType::ISX3_ACK_TYPE_COMMAND_ACKNOWLEDGE);
    readBuffer.clear();

    // Configure the frontend.
    auto setFeCmd = codec.buildCmdSetFeSettings(
        Devices::MeasurementConfiguration::MEAS_CONFIG_2_POINT,
        Devices::MeasurmentConfigurationChannel::MEAS_CONFIG_CHANNEL_EXT_PORT_2,
        Devices::MeasurmentConfigurationRange::MEAS_CONFIG_RANGE_10MA);
    socketWrapper->write(setFeCmd);
    // Check if acknowledgment has been received.
    readBuffer = readFromSocket(socketWrapper);
    buffer.pushBytes(readBuffer);
    auto setFeFrames = extractFrames(buffer);
    // There should be exactly one frame.
    REQUIRE(setFeFrames.size() == 1);
    // Decode the frame and check if it is an positive acknowledgment.
    auto setFeAckPayload = codec.decodeMessage(setFeFrames.front());
    REQUIRE(setFeAckPayload);
    auto FePayloadCasted =
        dynamic_pointer_cast<Devices::Isx3AckPayload>(setFeAckPayload);
    REQUIRE(FePayloadCasted);
    REQUIRE(FePayloadCasted->getAckType() ==
            Isx3AckType::ISX3_ACK_TYPE_COMMAND_ACKNOWLEDGE);
    readBuffer.clear();

    // Set Channels.
    auto setChannelsCommand =
        codec.buildCmdSetExtensionPortChannel(10, 10, 11, 11);
    socketWrapper->write(setChannelsCommand);
    // Check if acknowledgment has been received.
    readBuffer = readFromSocket(socketWrapper);
    buffer.pushBytes(readBuffer);
    auto setChannelsFrames = extractFrames(buffer);
    // There should be exactly one frame.
    REQUIRE(setChannelsFrames.size() == 1);
    // Decode the frame and check if it is an positive acknowledgment.
    auto setChannelsAckPayload = codec.decodeMessage(setChannelsFrames.front());
    REQUIRE(setChannelsAckPayload);
    auto SetChannelsPayloadCasted =
        dynamic_pointer_cast<Devices::Isx3AckPayload>(setChannelsAckPayload);
    REQUIRE(SetChannelsPayloadCasted);
    REQUIRE(SetChannelsPayloadCasted->getAckType() ==
            Isx3AckType::ISX3_ACK_TYPE_COMMAND_ACKNOWLEDGE);
    readBuffer.clear();

    // Disconnect again.
    bool disconnectSuccess = socketWrapper->close();
    REQUIRE(disconnectSuccess);
  }

  SECTION("Try to get measurement data") {
    // Note: This test assumes that a measurement setup was successfully
    // configured in the previous test.

    // Connect to device.
    bool connectionSuccess = socketWrapper->open("127.0.0.1", 8888);
    REQUIRE(connectionSuccess);

    // Start measurement
    std::vector<unsigned char> readBuffer;
    auto startMeasurementCommand =
        codec.buildCmdStartImpedanceMeasurement(true);
    socketWrapper->write(startMeasurementCommand);

    for (int i = 0; i < 10; i++) {
      // Check if acknowledgment has been received.
      readBuffer = readFromSocket(socketWrapper);
      buffer.pushBytes(readBuffer);
      auto frames = extractFrames(buffer);
      std::list<shared_ptr<ReadPayload>> payloads;
      for (auto frame : frames) {
        auto payload = codec.decodeMessage(frames.front());
        payloads.push_back(payload);
      }
    }

    // Disconnect again.
    bool disconnectSuccess = socketWrapper->close();
    REQUIRE(disconnectSuccess);
  }
}

TEST_CASE("Testing the implementation of the Sciospec ISX3 device") {
  DeviceIsx3 dut;

  // Init the device.
  shared_ptr<InitDeviceMessage>(
      new InitDeviceMessage(shared_ptr<MessageInterface>(),
                            new Isx3InitPayload("127.0.0.1", 8888), UserId()));
  dut.write(shared_ptr<InitDeviceMessage>());

  // Configure the device.

  dut.write(shared_ptr<ConfigDeviceMessage>());

  // Start the measurement.
  // dut.write(shared_ptr<DeviceMessage>());

  // Read measurement data.
  dut.read(TimePoint());
}
