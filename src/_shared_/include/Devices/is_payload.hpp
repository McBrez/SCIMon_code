#ifndef IS_PAYLOAD_HPP
#define IS_PAYLOAD_HPP

// Standard includes
#include <complex>
#include <list>
#include <tuple>

// Project includes
#include <read_payload.hpp>
#include <utilities.hpp>

using namespace std;
using namespace Utilities;

namespace Devices {
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

  unsigned int getChannelNumber();

  double getTimestamp();

  ImpedanceSpectrum getImpedanceSpectrum();

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