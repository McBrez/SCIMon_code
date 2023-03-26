// Standard includes
#include <format>

// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <device_isx3.hpp>
#include <id_payload.hpp>
#include <isx3_ack_payload.hpp>
#include <isx3_is_conf_payload.hpp>
#include <win_socket.hpp>

using namespace Devices;

INITIALIZE_EASYLOGGINGPP

/**
 * @brief Reads from the socket wrapper until no more bytes are received.
 * @param socketWrapper Pointer to the socket wrapper.
 * @return The bytes that have been read.
 */
std::vector<unsigned char>
readFromSocket(shared_ptr<SocketWrapper> socketWrapper, int retries = 10) {
  // Read until device no longer sends data.
  std::vector<unsigned char> readBuffer;
  int bytesRead = -1;
  for (int i = 0; i < retries; i++) {
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

#ifndef SKIP_TEST_BACKEND
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

    // Set to impedance spectrum measurement.
    auto setupCmd = codec.buildCmdInitMeasurement(
        false,
        MeasurementMode::MEASUREMENT_MODE_FULL_RANGE_IMPEDANCE_SPECTROSCOPY);
    socketWrapper->write(setupCmd);
    // Check if acknowledgment has been received.
    readBuffer = readFromSocket(socketWrapper);
    buffer.pushBytes(readBuffer);
    auto setupFrames = extractFrames(buffer);
    // There should be exactly one frame.
    REQUIRE(setupFrames.size() == 1);
    // Decode the frame and check if it is an positive acknowledgment.
    auto setupAckPayload = codec.decodeMessage(setupFrames.front());
    REQUIRE(setupAckPayload);
    auto setupAckPayloadCasted =
        dynamic_pointer_cast<Devices::Isx3AckPayload>(setupAckPayload);
    REQUIRE(setupAckPayloadCasted);
    REQUIRE(setupAckPayloadCasted->getAckType() ==
            Isx3AckType::ISX3_ACK_TYPE_COMMAND_ACKNOWLEDGE);
    readBuffer.clear();

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

    // Clear the frontend stack.
    auto clearFeCmd = codec.buildCmdClearFeSettings();
    socketWrapper->write(clearFeCmd);
    // Check if acknowledgment has been received.
    readBuffer = readFromSocket(socketWrapper);
    buffer.pushBytes(readBuffer);
    auto clearFeFrames = extractFrames(buffer);
    // There should be exactly one frame.
    REQUIRE(clearFeFrames.size() == 1);
    // Decode the frame and check if it is an positive acknowledgment.
    auto clearFeAckPayload = codec.decodeMessage(clearFeFrames.front());
    REQUIRE(clearFeAckPayload);
    auto clearFePayloadCasted =
        dynamic_pointer_cast<Devices::Isx3AckPayload>(clearFeAckPayload);
    REQUIRE(clearFePayloadCasted);
    REQUIRE(clearFePayloadCasted->getAckType() ==
            Isx3AckType::ISX3_ACK_TYPE_COMMAND_ACKNOWLEDGE);
    readBuffer.clear();

    // Configure the frontend.
    auto setFeCmd = codec.buildCmdSetFeSettings(
        Devices::MeasurementConfiguration::MEAS_CONFIG_4_POINT,
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

    // Set a few options Channels.
    /*auto setOptionCannelCmd = codec.buildCmdSetOptions(
        OptionType::OPTION_TYPE_ACTIVATE_CHANNEL_NUMBER, true);
    socketWrapper->write(setOptionCannelCmd);
    auto setOptionFrequencyCmd = codec.buildCmdSetOptions(
        OptionType::OPTION_TYPE_ACTIVATE_FREQUENCY_NUMBER, true);
    socketWrapper->write(setOptionFrequencyCmd);
    auto setOptionTimestampCmd = codec.buildCmdSetOptions(
        OptionType::OPTION_TYPE_ACTIVATE_TIMESTAMP, true);
    socketWrapper->write(setOptionTimestampCmd);*/
    // TODO: Check the ACKs here.
    // Clear the ACKS from buffer.
    std::vector<unsigned char> readBuffer = readFromSocket(socketWrapper);
    readBuffer.clear();

    // Start measurement
    auto startMeasurementCommand =
        codec.buildCmdStartImpedanceMeasurement(true);
    socketWrapper->write(startMeasurementCommand);
    // Check if acknowledgment has been received.
    // Note: The response to a start measurement command may take a bit longer.
    readBuffer = readFromSocket(socketWrapper, 50);
    buffer.pushBytes(readBuffer);
    auto startMeasurementFrames = extractFrames(buffer);
    // There should be at least one data frame. It is possible that the socket
    // already caught measurement data. However, at this stage only the first
    // response frame is evaluated, as it contains the ACK.
    REQUIRE(startMeasurementFrames.size() >= 1);
    // Decode the frame and check if it is an positive acknowledgment.
    auto startMeasurementAckPayload =
        codec.decodeMessage(startMeasurementFrames.front());
    REQUIRE(startMeasurementAckPayload);
    auto StartMeasurementPayloadCasted =
        dynamic_pointer_cast<Devices::Isx3AckPayload>(
            startMeasurementAckPayload);
    REQUIRE(StartMeasurementPayloadCasted);
    REQUIRE(StartMeasurementPayloadCasted->getAckType() ==
            Isx3AckType::ISX3_ACK_TYPE_COMMAND_ACKNOWLEDGE);
    readBuffer.clear();

    // Read from the socket for a few cycles and sollect the payload.
    std::list<shared_ptr<ReadPayload>> payloads;
    for (int i = 0; i < 10; i++) {
      // Check if acknowledgment has been received.
      readBuffer = readFromSocket(socketWrapper);
      buffer.pushBytes(readBuffer);
      auto frames = extractFrames(buffer);
      for (auto frame : frames) {
        string logString = "";
        for (auto frameByte : frame) {
          logString += std::format("{:#04x} ", frameByte);
        }
        LOG(INFO) << logString;

        auto payload = codec.decodeMessage(frame);
        REQUIRE(payload);
        payloads.push_back(payload);
      }
    }
    // There should be a few payloads now.
    REQUIRE(payloads.size() >= 1);
    // Each collected payload has to be an impedance spectrum.
    for (auto payload : payloads) {
      auto isPayload = dynamic_pointer_cast<IsPayload>(payload);
      REQUIRE(isPayload);
      LOG(INFO) << "Channel Number: " << isPayload->getChannelNumber() << " "
                << "Timestamp: " << isPayload->getTimestamp() << " "
                << "Frequency point: "
                << get<0>(isPayload->getImpedanceSpectrum().front()) << " "
                << "Impedance: "
                << get<1>(isPayload->getImpedanceSpectrum().front());
    }

    // Stop measurement.
    auto stopMeasurementCommand =
        codec.buildCmdStartImpedanceMeasurement(false);
    socketWrapper->write(stopMeasurementCommand);

    // Disconnect again.
    bool disconnectSuccess = socketWrapper->close();
    REQUIRE(disconnectSuccess);
  }
}
#endif

#define SKIP_TEST_DEVICEISX3
#ifndef SKIP_TEST_DEVICEISX3
TEST_CASE("Testing the implementation of the Sciospec ISX3 device") {
  shared_ptr<Device> dut(new DeviceIsx3());

  // Init the device.
  shared_ptr<InitDeviceMessage> initMsg(new InitDeviceMessage(
      shared_ptr<MessageInterface>(), new Isx3InitPayload("127.0.0.1", 8888),
      dut->getUserId()));
  bool initSuccess = dut->write(initMsg);
  REQUIRE(initSuccess);

  // Configure the device.
  shared_ptr<ConfigDeviceMessage> configMsg(new ConfigDeviceMessage(
      shared_ptr<MessageInterface>(),
      new Isx3IsConfPayload(
          10.0, 100.0, 10, 0,
          map<ChannelFunction, int>({{ChannelFunction::CHAN_FUNC_CP, 10},
                                     {ChannelFunction::CHAN_FUNC_RP, 10},
                                     {ChannelFunction::CHAN_FUNC_WS, 11},
                                     {ChannelFunction::CHAN_FUNC_WP, 11}}),
          IsScale::LINEAR_SCALE,
          MeasurmentConfigurationRange::MEAS_CONFIG_RANGE_10MA,
          MeasurmentConfigurationChannel::MEAS_CONFIG_CHANNEL_EXT_PORT,
          MeasurementConfiguration::MEAS_CONFIG_2_POINT, 1.0, 1.0)));
  bool configSuccess = dut->write(configMsg);
  REQUIRE(configSuccess);

  // Start the measurement.
  shared_ptr<WriteDeviceMessage> startMsg(new WriteDeviceMessage(
      shared_ptr<MessageInterface>(), dut, WriteDeviceTopic::WRITE_TOPIC_RUN));
  bool startSuccess = dut->write(startMsg);
  REQUIRE(startSuccess);

  // Read measurement data.
  int readCounter = 1000;
  list<shared_ptr<DeviceMessage>> readList;
  for (int i = 0; i < readCounter; i++) {
    readList.merge(dut->read(TimePoint()));
    this_thread::sleep_for(chrono::microseconds(10));
  }
  REQUIRE(!readList.empty());

  // Stop the measurement.
  shared_ptr<WriteDeviceMessage> stopMsg(new WriteDeviceMessage(
      shared_ptr<MessageInterface>(), dut, WriteDeviceTopic::WRITE_TOPIC_STOP));
  bool stopSuccess = dut->write(stopMsg);
  REQUIRE(stopSuccess);
}
#endif