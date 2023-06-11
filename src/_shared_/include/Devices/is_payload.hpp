#ifndef IS_PAYLOAD_HPP
#define IS_PAYLOAD_HPP

// Standard includes
#include <complex>
#include <list>
#include <tuple>

// Project includes
#include <common.hpp>
#include <read_payload.hpp>

using namespace Core;

namespace Devices {
/**
 * @brief Encapsulates a discrete impedance spectrum into a message payload.
 */
class IsPayload : public ReadPayload {
public:
  /**
   * @brief Constructs the object with the given parameter.
   *
   * @param channelNumber The number of the channel the impedance spectrum has
   * been gathered from.
   * @param timestamp The timestamp of the measurement in UNIX time.
   * @param impedanceSpectrum The impedance spectrum.
   */
  IsPayload(unsigned int channelNumber, double timestamp,
            ImpedanceSpectrum impedanceSpectrum);

  /**
   * @brief Constructs the object with the given parameter.
   *
   * @param channelNumber The number of the channel the impedance spectrum has
   * been gathered from.
   * @param timestamp The timestamp of the measurement in UNIX time.
   * @param frequencies std::list of frequencies that match to impedances.
   * @param impedances std::list of impedances that match to frequencies.
   */
  IsPayload(unsigned int channelNumber, double timestamp,
            std::list<double> frequencies,
            std::list<std::complex<double>> impedances);

  unsigned int getChannelNumber() const;

  double getTimestamp() const;

  ImpedanceSpectrum getImpedanceSpectrum() const;

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual std::string serialize() override;

  /**
   * @brief Serializes the payload into bytes.
   * @return Byte vector, that depicts the payload.
   */
  virtual std::vector<unsigned char> bytes() override;

  /**
   * @brief Returns the magic number of the payload. This number is used to
   * identify the payload type when encoding and decoding payload. This number
   * has to be unique among all payload types.
   */
  virtual int getMagicNumber() override;

private:
  /// The number of the channel the impedance spectrum has been gathered from.
  unsigned int channelNumber;
  /// The timestamp of the measurement in UNIX time.
  double timestamp;
  /// The impedance spectrum.
  ImpedanceSpectrum impedanceSpectrum;
};
} // namespace Devices

#endif