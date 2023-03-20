#ifndef COM_INTERFACE_CODEC_HPP
#define COM_INTERFACE_CODEC_HPP

// Standard includes
#include <complex>
#include <list>
#include <vector>

// Project includes
#include <configuration_payload.hpp>
#include <device_message.hpp>
#include <init_payload.hpp>
#include <isx3_constants.hpp>
#include <read_payload.hpp>

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
   * @brief Builds a "Set FE Settings" command from the given arguments.
   * @param measConf The measurement configuration mode that shall be set.
   * @param measConfChannel The front end channel that shall be used.
   * @param measConfRange The front end measurement range that shall be used.
   * @return A "Set FE Settings" command that represents the given arguments.
   */
  std::vector<unsigned char>
  buildCmdSetFeSettings(MeasurementConfiguration measConf,
                        MeasurmentConfigurationChannel measConfChannel,
                        MeasurmentConfigurationRange measConfRange);

  /**
   * @brief Builds an "Set ExtensionPort Channel" command from the given
   * arguments.
   * @param counterPort The number of the counter port.
   * @param referencePort The number of the reference port.
   * @param workingSensePort The number of the working sense port.
   * @param workPort The number of the work port.
   * @return A "Set ExtensionPort Channel" command .
   */
  std::vector<unsigned char>
  buildCmdSetExtensionPortChannel(int counterPort, int referencePort,
                                  int workingSensePort, int workPort);

  // ------------------------------------------------- Impedance Measurement --

  /**
   * @brief Build a Set Setup command with the "Init Measurement" command
   * option.
   * @return An "Init Measurement" Set Setup command.
   */
  std::vector<unsigned char> buildCmdSetSetup();

  /**
   * @brief Build a Set Setup command with the "Add single frequency point"
   * command option.
   * @param frequencyPoint The frequency point that shall be added.
   * @param precision The precision of the measurement. Has to be within 0.0
   * and 10.0.
   * @param amplitude The peak amplitude of the sinusoidal excitation. Has to
   * within the technical capabilities of the device.
   * @return An "Add single frequency point" Set Setup command.
   */
  std::vector<unsigned char> buildCmdSetSetup(float frequencyPoint,
                                              float precision, float amplitude);

  /**
   * @brief Build a Set Setup command with the "Add frequency list"
   * command option.
   * @param fStart Start frequency of sweep.
   * @param fStop Stop frequency of sweep.
   * @param fCount Number of frequency points in between f_start and f_stop.
   * Value is rounded down.
   * @param frequencyScale Determines if a linear or a logarithmic distribution
   * of frequencies is used.
   * @param precision The precision of the measurement. Has to be within 0.0
   * and 10.0.
   * @param amplitude The peak amplitude of the sinusoidal excitation. Has to
   * within the technical capabilities of the device.
   * @return An "Add frequency list" Set Setup command.
   */
  std::vector<unsigned char> buildCmdSetSetup(float fStart, float fStop,
                                              float fCount,
                                              FrequencyScale frequencyScale,
                                              float precision, float amplitude);

  /**
   * @brief Builds a "Start Impedance Measurement" command that may start or
   * stop the measurement.
   * @param state When TRUE, the measurment will be started. When FALSE a
   * currently active measurement will be stoped.
   * @return A "Start Impedance Measurement" that may start or top the
   * measurement.
   */
  std::vector<unsigned char> buildCmdStartImpedanceMeasurement(bool state);

  /**
   * @brief Builds a "Start Impedance Measurement" command that enables the
   * measurement for the specified burst count.
   * @param burstCount Specifies the count of impedance spectra that shall be
   * measured until the measuremnt stops again. Has to be 2 bytes long (i.e.
   * data type 'short' on most platforms).
   * @return A "Start Impedance Measurement" command that enables the
   * measurement for the specified burst count.
   */
  std::vector<unsigned char>
  buildCmdStartImpedanceMeasurement(unsigned short burstCount);

  // ---------------------------------------------- Message specific methods --

  /**
   * @brief Decodes the given raw data, that has been gathered from a device,
   * and parses it to a payload object.
   * @param bytes The raw data that shall be parsed.
   * @return A shared pointer to the payload object.
   */
  shared_ptr<ReadPayload> decodeMessage(std::vector<unsigned char> bytes);

  /**
   * @brief Encodes an init payload into a COM interface data frame.
   * @param payload The payload that shall be encoded.
   * @return The encoded COM interface data frame.
   */
  std::vector<unsigned char> encodeMessage(shared_ptr<InitPayload> payload);

  /**
   * @brief Encodes a config payload into a list of COM interface data frames.
   * @param payload The payload that shall be encoded.
   * @return A list of COM interface data frames.
   */
  std::list<std::vector<unsigned char>>
  encodeMessage(shared_ptr<ConfigurationPayload> payload);

private:
  /**
   * @brief Wraps the given payload with the ISX3 COM interface frame.
   * @param payload The payload that shall be put into the frame.
   * @param
   * @return The ISX3 COM interface frame, containing the given payload.
   */
  std::vector<unsigned char>
  wrapPayload(const std::vector<unsigned char> &payload, Isx3CmdTag cmdTag);

  /**
   * @brief Consumes a COM interface frame and extracts the payload from it.
   * Also does a consistency check.
   * @param frame The COM interface frame that shall be unwrapped.
   * @param payload Will contain the payload.
   * @param commandTag will contain the command tag.
   * @return True if a valid payload could be extracted. False otherwise.
   */
  bool unwrapPayload(const std::vector<unsigned char> &frame,
                     std::vector<unsigned char> &payload,
                     Isx3CmdTag &commandTag);

  /**
   * @brief Returns the raw vytes of given value.
   * @param value The value which shall be converted to raw bytes.
   * @param bigEndian Determines the order of the bytes in the returned vecotr.
   * TRUE -> MSB is placed at the lowest index. FALSE -> MSB is placed at the
   * highest index.
   * @return The raw bytes of the given value.
   */
  template <typename T>
  std::vector<unsigned char> getRawBytes(T value, bool bigEndian = true);

  /**
   * @brief Decodes impedance spectroscopy measurement data sent by the device.
   * @param payload The payload that shall be decoded.
   * @param fNumber Will contain the frequency index of the measurement.
   * @param timestamp Will contain The timestamp of the measurement (in
   * milliseconds).
   * @param channelNumber Will contain the channelNumber of the measurement.
   * @param impedance Will contain the complex impedance that has been measured
   * by the device (in Ohm).
   * @return TRUE if the payload could be successfully decoded. FALSE otherwise.
   */
  bool decodeImpedanceData(const std::vector<unsigned char> &payload,
                           short &fNumber, float &timestamp,
                           short &channelNumber,
                           std::complex<float> &impedance);

  /**
   * @brief Decodes an acknowledgment payload received from the device.
   *
   * @param payload The payload that shall be decoded.
   * @param ackType The acknowledgment type.
   * @return TRUE if payload has been decoded successfully. FALSE otherwise.
   */
  bool decodeAck(const std::vector<unsigned char> &payload,
                 Isx3AckType &ackType);

  /**
   * @brief Decodes an "GetDeviceId" frame.
   *
   * @param payload The payload that shall be decoded.
   * @param formatVersion The version of the device id format.
   * @param deviceIdentifier The identifier for the device type.
   * @param serialNumber The serial number of the device.
   * @param dateOfDeliveryYear The year the device has been delivered.
   * @param dateOfDeliveryMonth The month the device has been delivered.
   * @return TRUE if frame has been decoded succesfully. False otherwise.
   */
  bool decodeDeviceId(const std::vector<unsigned char> &payload,
                      unsigned char &formatVersion, short &deviceIdentifier,
                      short &serialNumber, int &dateOfDeliveryYear,
                      int &dateOfDeliveryMonth);
};
} // namespace Devices

#endif