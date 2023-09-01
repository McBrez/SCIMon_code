// Project includes
#include <utilities_flatbuffers.hpp>

namespace Utilities {

KeyMapping buildKeyMappingFromFlatbuffers(
    const std::vector<
        std::unique_ptr<Serialization::Messages::KeyMappingEntryT>>
        &fbKeyMapping) {

  // The key mapping.
  KeyMapping keyMapping;
  for (auto &keyMappingEntry : fbKeyMapping) {
    keyMapping[keyMappingEntry->keys] =
        static_cast<DataManagerDataType>(keyMappingEntry->dataTypes);
  }

  return keyMapping;
}

SpectrumMapping buildSpectrumMappingFromFlatbuffers(
    const std::vector<
        std::unique_ptr<Serialization::Messages::SpectrumMappingEntryT>>
        &fbSpectrumMapping) {

  // The spectrum mapping.
  SpectrumMapping spectrumMapping;
  for (auto &spectrumMappingEntry : fbSpectrumMapping) {
    spectrumMapping[spectrumMappingEntry->key] =
        spectrumMappingEntry->frequencies;
  }

  return spectrumMapping;
}
} // namespace Utilities
