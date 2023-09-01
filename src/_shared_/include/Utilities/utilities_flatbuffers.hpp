#ifndef UTILITIES_FLATBUFFER_HPP
#define UTILITIES_FLATBUFFER_HPP

// Project includes
#include <data_manager.hpp>

// Generated includes
#include <config_device_message_content_generated.h>

namespace Utilities {

/**
 * @brief Constructs a key mapping from the flatbuffers keymapping
 * implementations.
 * @param keyMapping The flatbuffers-style keymapping.
 * @return A proper key mapping.
 */
KeyMapping buildKeyMappingFromFlatbuffers(
    const std::vector<
        std::unique_ptr<Serialization::Messages::KeyMappingEntryT>>
        &fbKeyMapping);

/**
 * @brief Constructs a spectrum mapping from the flatbuffers spectrum mapping
 * implementations.
 * @param fbSpectrumMapping The flatbuffers-style spectrum mapping.
 * @return A proper spectrum mapping.
 */
SpectrumMapping buildSpectrumMappingFromFlatbuffers(
    const std::vector<
        std::unique_ptr<Serialization::Messages::SpectrumMappingEntryT>>
        &fbSpectrumMapping);

} // namespace Utilities

#endif
