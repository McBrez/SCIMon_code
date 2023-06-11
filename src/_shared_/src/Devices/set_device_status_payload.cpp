// Project includes
#include <common.hpp>
#include <set_device_status_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <set_device_state_payload_generated.h>

using namespace Devices;
using namespace Messages;

SetDeviceStatusPayload::SetDeviceStatusPayload(UserId targetId, bool setStatus)
    : targetId(targetId), setStatus(setStatus) {}

SetDeviceStatusPayload::~SetDeviceStatusPayload() {}

std::string SetDeviceStatusPayload::serialize() { return ""; }

std::vector<unsigned char> SetDeviceStatusPayload::bytes() {
  Serialization::Devices::SetDeviceStatePayloadT intermediateObject;

  intermediateObject.setStatus = this->setStatus;
  intermediateObject.targetDeviceId = this->targetId.id();

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::SetDeviceStatePayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int SetDeviceStatusPayload::getMagicNumber() {
  return MAGIC_NUMBER_SET_DEVICE_STATUS_PAYLOAD;
}
