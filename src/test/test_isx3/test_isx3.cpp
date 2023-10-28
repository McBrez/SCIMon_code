// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <device_isx3.hpp>
#include <isx3_is_conf_payload.hpp>
#include <message_distributor.hpp>

using namespace Devices;

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Test the ISX3 Device.") {
  // Build up logic.
  const std::string comPort = "COM1";
  const int baudRate = 182000;
  std::shared_ptr<DeviceIsx3> dut(new DeviceIsx3());
  MessageDistributor messageDistributor(std::chrono::milliseconds(500));
  std::thread messageDistributorWorker(&MessageDistributor::run,
                                       &messageDistributor);

  SECTION("Establish connection to COM Port") {

    std::shared_ptr<InitPayload> initPayload(
        new Isx3InitPayload(comPort, baudRate));
    std::shared_ptr<InitDeviceMessage> initMsg(
        new InitDeviceMessage(UserId(), dut->getUserId(), initPayload));
    bool writeSuccess = dut->write(initMsg);

    REQUIRE(writeSuccess);
    REQUIRE(dut->getDeviceStatus() == DeviceStatus::INITIALIZED);

    SECTION("Configure the Device") {
      std::shared_ptr<ConfigurationPayload> configPayload(new Isx3IsConfPayload(
          10.0, 1000.0, 10, 0, std::map<ChannelFunction, int>(),
          IsScale::LINEAR_SCALE,
          MeasurmentConfigurationRange::MEAS_CONFIG_RANGE_100UA,
          MeasurmentConfigurationChannel::MEAS_CONFIG_CHANNEL_EXT_PORT_2,
          MeasurementConfiguration::MEAS_CONFIG_2_POINT, 1.0, 1.0));
      std::shared_ptr<ConfigDeviceMessage> configMsg(
          new ConfigDeviceMessage(UserId(), dut->getUserId(), configPayload));
      bool writeSuccess = dut->write(configMsg);

      REQUIRE(writeSuccess);
      REQUIRE(dut->getDeviceStatus() == DeviceStatus::IDLE);

      SECTION("Start the Measurement") {
        std::shared_ptr<WriteDeviceMessage> startMsg(new WriteDeviceMessage(
            UserId(), dut->getUserId(), WriteDeviceTopic::WRITE_TOPIC_RUN));
        bool writeSuccess = dut->write(startMsg);

        REQUIRE(writeSuccess);
        REQUIRE(dut->getDeviceStatus() == DeviceStatus::OPERATING);
      }
    }
  }

  // Tear down logic.
  messageDistributor.stop();
  messageDistributorWorker.join();
}
