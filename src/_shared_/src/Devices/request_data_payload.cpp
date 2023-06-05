// Project includes
#include <request_data_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <request_data_payload_generated.h>

using namespace Devices;

RequestDataPayload::RequestDataPayload(DeviceType deviceType, TimePoint from,
                                       TimePoint to)
    : deviceType(deviceType), from(from), to(to) {}

RequestDataPayload::~RequestDataPayload() {}

string RequestDataPayload::serialize() { return ""; }

vector<unsigned char> RequestDataPayload::bytes() {
  Serialization::Devices::RequestDataPayloadT intermediateObject;
  intermediateObject.deviceType =
      static_cast<Serialization::Devices::DeviceType>(this->deviceType);
  intermediateObject.from = this->from.time_since_epoch().count();
  intermediateObject.to = this->from.time_since_epoch().count();

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::RequestDataPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int RequestDataPayload::getMagicNumber() {
  return MAGIC_NUMBER_REQUEST_DATA_PAYLOAD;
}