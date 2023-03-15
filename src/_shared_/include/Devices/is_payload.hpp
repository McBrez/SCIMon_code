#ifndef IS_PAYLOAD_HPP
#define IS_PAYLOAD_HPP

// Standard includes
#include <complex>
#include <list>
#include <tuple>

// Project includes
#include <common.hpp>
#include <read_payload.hpp>

using namespace std;
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
   * @param frequencies List of frequencies that match to impedances.
   * @param impedances List of impedances that match to frequencies.
   */
  IsPayload(unsigned int channelNumber, double timestamp,
            list<double> frequencies, list<complex<double>> impedances);

  unsigned int getChannelNumber() const;

  double getTimestamp() const;

  ImpedanceSpectrum getImpedanceSpectrum() const;

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual string serialize() override;

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