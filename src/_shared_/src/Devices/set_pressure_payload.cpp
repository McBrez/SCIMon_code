// Projects
#include <common.hpp>
#include <set_pressure_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <set_pressure_payload_generated.h>

using namespace Devices;

SetPressurePayload::SetPressurePayload(const std::vector<double> &pressures,
                                       PressureUnit pressureUnit)
    : pressures(pressures), pressureUnit(pressureUnit) {}

SetPressurePayload::~SetPressurePayload() {}

std::string SetPressurePayload::serialize() { return ""; }

std::vector<double> SetPressurePayload::getPressures() const {
  return this->pressures;
}

PressureUnit SetPressurePayload::getPressureUnit() const {
  return this->pressureUnit;
}

std::vector<unsigned char> SetPressurePayload::bytes() {
  Serialization::Devices::SetPressurePayloadT intermediateObject;
  intermediateObject.pressures = this->pressures;
  intermediateObject.pressureUnit =
      static_cast<Serialization::Devices::PressureUnit>(this->pressureUnit);

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::SetPressurePayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int SetPressurePayload::getMagicNumber() {
  return MAGIC_NUMBER_SET_PRESSURE_PAYLOAD;
}
