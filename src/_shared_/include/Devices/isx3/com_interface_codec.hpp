#ifndef COM_INTERFACE_CODEC_HPP
#define COM_INTERFACE_CODEC_HPP

// Standard includes
#include <vector>

// Project includes
#include <device_message.hpp>

using namespace Messages;

namespace Devices {

/**
 * @brief Identifies measurements modes as used by the init measurement command.
 */
enum MeasurementMode {
  MEASUREMENT_MODE_INVALID = 0xFF,
  MEASUREMENT_MODE_EFP = 0x00,
  MEASUREMENT_MODE_EFP_STIMULATION = 0x01,
  MEASUREMENT_MODE_IMPEDANCE_SPECTROSCOPY = 0x02,
  MEASUREMENT_MODE_FULL_RANGE_IMPEDANCE_SPECTROSCOPY = 0x03
};

/**
 * @brief Encapsulates a Codec that adheres to the COM interface defined by
 * Sciospec.
 */
class ComInterfaceCodec {
public:
  static const unsigned char CmdTagAck = 0x18;
  static const unsigned char CmdTagResetSystem = 0xA1;
  static const unsigned char CmdTagGetDeviceId = 0xD1;
  static const unsigned char CmdTagInitMeasurement = 0xB4;

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
   * @brief Builds a Reset System command.
   * @return An Reset System command.
   */
  std::vector<unsigned char> buildCmdResetSystem();

  /**
   * @brief Builds a Get Device ID command.
   * @return An Get Device ID command.
   */
  std::vector<unsigned char> buildCmdGetDeviceId();

  /**
   * @brief Builds an Init Measurement command from the given arguments.
   * @param readWrite Specifies whether the measurement mode shall be read from
   * the device, or shall be written to it. TRUE corresponds to read, FALSE to
   * write.
   * @param measurementMode The measurement mode that shall be set. Ignored if
   * readWrite == TRUE.
   * @return An Init Measurement command.
   */
  std::vector<unsigned char>
  buildCmdInitMeasurement(bool readWrite, MeasurementMode measurementMode);

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