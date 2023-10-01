// Project includes
#include <common.hpp>
#include <key_response_payload.hpp>

// Generated includes
#include <key_response_payload_generated.h>

using namespace Utilities;

namespace Devices {

KeyResponsePayload::KeyResponsePayload(const KeyMapping &keyMapping,
                                       const SpectrumMapping &spectrumMapping)
    : keyMapping(keyMapping), spectrumMapping(spectrumMapping) {}

KeyResponsePayload::~KeyResponsePayload() {}

std::string KeyResponsePayload::serialize() { return ""; }

std::vector<unsigned char> KeyResponsePayload::bytes() {
  Serialization::Devices::KeyResponsePayloadT intermediateObject;

  // Parse the key mapping.
  for (auto &keyTypeMapping : this->keyMapping) {

    Serialization::Devices::TypeMappingEntryT *typeMappingEntry =
        new Serialization::Devices::TypeMappingEntryT();
    typeMappingEntry->key = keyTypeMapping.first;
    typeMappingEntry->type = static_cast<unsigned int>(keyTypeMapping.second);

    intermediateObject.typeMappings.emplace_back(
        std::unique_ptr<Serialization::Devices::TypeMappingEntryT>(
            typeMappingEntry));
  }

  // Parse the spectrum mapping.
  for (auto &entry : this->spectrumMapping) {

    Serialization::Devices::SpectrumMappingEntryT *spectrumMappingEntry =
        new Serialization::Devices::SpectrumMappingEntryT();
    spectrumMappingEntry->key = entry.first;
    spectrumMappingEntry->frequencies = entry.second;

    intermediateObject.spectrumMapping.emplace_back(
        std::unique_ptr<Serialization::Devices::SpectrumMappingEntryT>(
            spectrumMappingEntry));
  }

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::KeyResponsePayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int KeyResponsePayload::getMagicNumber() {
  return MAGIC_NUMBER_KEY_RESPONSE_PAYLOAD;
}

KeyMapping &KeyResponsePayload::getKeyMapping() { return this->keyMapping; }

SpectrumMapping &KeyResponsePayload::getSpectrumMapping() {
  return this->spectrumMapping;
}
} // namespace Devices
