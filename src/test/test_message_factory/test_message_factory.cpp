// Standard includes
#include <format>

// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <dummy_device.hpp>
#include <handshake_message.hpp>
#include <message_factory.hpp>

INITIALIZE_EASYLOGGINGPP

using namespace Messages;
using namespace Devices;

TEST_CASE("Test the encoding and decoding of messages") {
  MessageFactory dut;

  shared_ptr<Device> dummySource(new DummyDevice());
  shared_ptr<Device> dummyDestination(new DummyDevice());

  SECTION("Handshake Message") {
    list<shared_ptr<StatusPayload>> statusPayloads;
    statusPayloads.emplace_back(shared_ptr<StatusPayload>(
        new StatusPayload(UserId(1), DeviceStatus::INITIALIZING, list<UserId>(),
                          DeviceType::IMPEDANCE_SPECTROMETER, "Device1")));
    statusPayloads.emplace_back(shared_ptr<StatusPayload>(
        new StatusPayload(UserId(2), DeviceStatus::OPERATING, list<UserId>(),
                          DeviceType::PUMP_CONTROLLER, "Device2")));
    statusPayloads.emplace_back(shared_ptr<StatusPayload>(
        new StatusPayload(UserId(3), DeviceStatus::ERROR,
                          list<UserId>{UserId(11), UserId(22), UserId(33)},
                          DeviceType::PUMP_CONTROLLER, "Device3")));

    shared_ptr<DeviceMessage> handshakeMessage(
        new HandshakeMessage(dummySource->getUserId(),
                             dummyDestination->getUserId(), statusPayloads));
    auto messageOrig = dynamic_pointer_cast<HandshakeMessage>(handshakeMessage);
    vector<unsigned char> encodedMessage = dut.encodeMessage(handshakeMessage);

    shared_ptr<DeviceMessage> decodedMessage =
        dut.decodeMessage(encodedMessage);
    auto result = dynamic_pointer_cast<HandshakeMessage>(decodedMessage);
    REQUIRE(result);
    REQUIRE(result->getDestination() == messageOrig->getDestination());
    REQUIRE(result->getSource() == messageOrig->getSource());

    list<shared_ptr<StatusPayload>> origPayloads = messageOrig->getPayload();
    list<shared_ptr<StatusPayload>> resultPayloads = result->getPayload();
    auto resultPayloadsIt = resultPayloads.begin();
    for (auto origPayload : origPayloads) {
      REQUIRE(*origPayload == **resultPayloadsIt);
      ++resultPayloadsIt;
    }
  }
}
