// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <Elveflow64.h>
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <device_ob1_win.hpp>
#include <init_payload_ob1.hpp>
#include <read_payload_ob1.hpp>
#include <status_payload.hpp>
#include <write_message_ob1.hpp>

using namespace Devices;
using namespace Messages;

INITIALIZE_EASYLOGGINGPP

TEST_CASE("Testing the implementation of the ElveFlow OB1 device",
          "[Devices]") {
  // Build up
  shared_ptr<MessageInterface> dut;
#ifdef WIN32
  dut.reset(new DeviceOb1Win());
#else
#error Target operating systems other than WIN32 are not yet supported.
#endif
  // Prepare a downcasted version of the dut.
  shared_ptr<DeviceOb1> downcastedDut = dynamic_pointer_cast<DeviceOb1>(dut);
  REQUIRE(downcastedDut);

  SECTION("Initializing") {
    // Create init payload and init message and send it to the DUT.
    InitPayloadOb1 *initPayload = new InitPayloadOb1(
        "01F8E63F",
        make_tuple(Z_regulator_type__0_2000_mbar, Z_regulator_type__0_2000_mbar,
                   Z_regulator_type_none, Z_regulator_type_none));
    shared_ptr<InitDeviceMessage> initMsg(new InitDeviceMessage(
        shared_ptr<MessageInterface>(), initPayload, dut->getUserId()));
    REQUIRE(dut->write(initMsg) == true);

    // A read should return an empty message.
    auto ackMsg = dut->read(chrono::system_clock::now());
    REQUIRE(ackMsg.empty());

    // Configure the DUT.
    shared_ptr<ConfigDeviceMessage> configMsg(new ConfigDeviceMessage(
        shared_ptr<MessageInterface>(),
        new ConfigurationPayload(DeviceType::PUMP_CONTROLLER)));
    REQUIRE(dut->write(configMsg));

    // Configuration may take a while. Query the DUT until configuration
    // finishes.
    list<shared_ptr<DeviceMessage>> readMessages;
    bool run = true;
    while (run) {
      this_thread::sleep_for(chrono::seconds(1));
      shared_ptr<WriteDeviceMessage> queryStateMsg(
          new WriteDeviceMessage(shared_ptr<MessageInterface>(), dut,
                                 WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE));
      // Query state messages should always be successful.
      REQUIRE(dut->write(queryStateMsg));
      readMessages = dut->read(chrono::system_clock::now());
      // Query state messages should immediately return with a single message.
      REQUIRE(readMessages.size() == 1);
      // Cast the message.
      shared_ptr<ReadDeviceMessage> readMsg =
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
        run = false;
      }
    }
    if (true) {
      REQUIRE(downcastedDut->start());
      shared_ptr<WriteDeviceMessage> setPressureMsg(
          new WriteMessageOb1SetPressure(
              shared_ptr<MessageInterface>(), dut,
              map<int, double>({{0, 0.0}, {1, 0.0}})));
      REQUIRE(dut->write(setPressureMsg));

      for (int i = 0; i < 10; i++) {
        auto specificReadMessages = dut->read(chrono::system_clock::now());
        // There should be exactly one message in the response list.
        REQUIRE(specificReadMessages.size() == 1);
        // Downcast the message.

        auto specificReadMessage = dynamic_pointer_cast<ReadDeviceMessage>(
            specificReadMessages.front());
        REQUIRE(specificReadMessage);
        auto payload = dynamic_pointer_cast<ReadPayloadOb1>(
            specificReadMessage->getReadPaylod());
        REQUIRE(payload);
        LOG(INFO) << payload->serialize();
        this_thread::sleep_for(chrono::seconds(1));
      }
    }
  }
}
