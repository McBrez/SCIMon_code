// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <Elveflow64_shim.h>
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <device_ob1_win.hpp>
#include <ob1_conf_payload.hpp>
#include <ob1_init_payload.hpp>
#include <read_payload_ob1.hpp>
#include <set_pressure_payload.hpp>
#include <status_payload.hpp>

using namespace Devices;
using namespace Messages;

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing the implementation of the ElveFlow OB1 device",
          "[Devices]") {
  // Build up
  std::shared_ptr<MessageInterface> dut;
#ifdef WIN32
  dut.reset(new DeviceOb1Win());
#else
#error Target operating systems other than WIN32 are not yet supported.
#endif
  // Prepare a downcasted version of the dut.
  std::shared_ptr<DeviceOb1> downcastedDut =
      dynamic_pointer_cast<DeviceOb1>(dut);
  REQUIRE(downcastedDut);

  SECTION("Initialize the OB1 Device") {
    // Create init payload and init message and send it to the DUT.
    Ob1InitPayload *initPayload = new Ob1InitPayload(
        "01FB0FA3",
        std::make_tuple(Z_regulator_type_m1000_1000_mbar, Z_regulator_type_none,
                        Z_regulator_type_none, Z_regulator_type_none));
    std::shared_ptr<InitDeviceMessage> initMsg(
        new InitDeviceMessage(UserId(), dut->getUserId(), initPayload));
    REQUIRE(dut->write(initMsg) == true);

    // A read should return an empty message.
    auto ackMsg = dut->read(Core::getNow());
    REQUIRE(ackMsg.empty());

    SECTION("Configure the OB1 device") {}

    // Configure the DUT.
    std::shared_ptr<ConfigDeviceMessage> configMsg(new ConfigDeviceMessage(
        UserId(), dut->getUserId(), new Ob1ConfPayload()));
    REQUIRE(dut->write(configMsg));

    // Configuration may take a while. Query the DUT until configuration
    // finishes.
    std::list<std::shared_ptr<DeviceMessage>> readMessages;
    bool run = true;
    while (run) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::shared_ptr<WriteDeviceMessage> queryStateMsg(
          new WriteDeviceMessage(UserId(), dut->getUserId(),
                                 WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE));
      // Query state messages should always be successful.
      REQUIRE(dut->write(queryStateMsg));
      readMessages = dut->read(Core::getNow());
      // Query state messages should immediately return with a single message.
      REQUIRE(readMessages.size() == 1);
      // Cast the message.
      std::shared_ptr<ReadDeviceMessage> readMsg =
          dynamic_pointer_cast<ReadDeviceMessage>(readMessages.front());
      REQUIRE(readMsg);
      // Check if this is the response to the previously sent query state
      // message.
      REQUIRE(readMsg->getOriginalMessage()->getMessageId() ==
              queryStateMsg->getMessageId());
      // Check if the response has the correct topic.
      REQUIRE(readMsg->getTopic() == ReadDeviceTopic::READ_TOPIC_DEVICE_STATUS);
      // Check if calibration already finished.
      auto statusPayload =
          dynamic_pointer_cast<StatusPayload>(readMsg->getReadPaylod());
      // Cast should be succesful.
      REQUIRE(statusPayload);
      if (statusPayload->getDeviceStatus() == DeviceStatus::IDLE) {
        // An IDLE state indicates, that the device finished configuration. Exit
        // this loop.
        run = false;
      }
    }

    SECTION("Start the OB1 device") {
      // Send the start message. The physical device should start with the last
      // set pressure values.
      bool startMessageSuccess =
          dut->write(std::shared_ptr<WriteDeviceMessage>(new WriteDeviceMessage(
              UserId(), dut->getUserId(), WriteDeviceTopic::WRITE_TOPIC_RUN)));
      REQUIRE(startMessageSuccess);

      // Set new pressure values.
      SetPressurePayload *setPressurePayload =
          new SetPressurePayload({1.0, 0.0, 0.0, 0.0}, PressureUnit::BAR);
      std::shared_ptr<WriteDeviceMessage> setPressureMsg(new WriteDeviceMessage(
          UserId(), dut->getUserId(),
          WriteDeviceTopic::WRITE_TOPIC_DEVICE_SPECIFIC, setPressurePayload));
      REQUIRE(dut->write(setPressureMsg));

      // Reading from the device should produce message containing the actual
      // pressure values. Do that for a few iterations.
      for (int i = 0; i < 10; i++) {
        auto specificReadMessages = dut->read(Core::getNow());
        // There should be exactly one message in the response list.
        REQUIRE(specificReadMessages.size() == 1);
        // Downcast the message. It should be a ReadDeviceMessage.
        auto specificReadMessage = std::dynamic_pointer_cast<ReadDeviceMessage>(
            specificReadMessages.front());
        REQUIRE(specificReadMessage);
        // The ReadDeviceMessage should hold a ReadPayladOb1 payload.
        auto payload = std::dynamic_pointer_cast<ReadPayloadOb1>(
            specificReadMessage->getReadPaylod());
        REQUIRE(payload);
        // Write it to output.
        LOG(INFO) << payload->serialize();
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
  }
}
