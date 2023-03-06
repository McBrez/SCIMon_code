#ifndef COM_INTERFACE_CODEC_HPP
#define COM_INTERFACE_CODEC_HPP

// Standard includes
#include <vector>

// Project includes
#include <device_message.hpp>

using namespace Messages;

namespace Devices {
/**
 * @brief Encapsulates a Codec that adheres to the COM interface defined by
 * Sciospec.
 */
class ComInterfaceCodec {
public:
  /**
   * @brief Construct a new Com Interface Codec object
   */
  ComInterfaceCodec();

  /**
   * @brief Builds a XYZ command from the given arguments.
   * @return std::vector<unsigned char>
   */
  std::vector<unsigned char> buildCmdXYZ();

/**
 * @brief Decodes the given raw data and parses it to a device message.
 * @param bytes The raw data that shall be parsed.
 * @return shared_ptr<DeviceMessage> The parsed message.
 */
  shared_ptr<DeviceMessage> decodeMessage(std::vector<unsigned char> bytes);

/**
 * @brief Encodes the given device message to a byte vector.
 * @param message The message that shall be parsed.
 * @return The encoded byte vector.
 */
  std::vector<unsigned char> encodeMessage(shared_ptr<DeviceMessage> message);
};
} // namespace Devices

#endif