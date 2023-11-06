#ifndef SET_PRESSURE_PAYLOAD_HPP
#define SET_PRESSURE_PAYLOAD_HPP

// Standard includes
#include <vector>

// Project includes
#include <write_payload.hpp>

namespace Devices {

enum PressureUnit { PSI = 0, BAR = 1 };

/**
 * @brief Encapsulates the set point pressures of an pump controller.
 */
class SetPressurePayload : public WritePayload {
public:
  /**
   * @brief Construct a new ReadPayload object.
   */
  SetPressurePayload(const std::vector<double> &pressures,
                     PressureUnit pressureUnit);

  /**
   * @brief Destroy the ReadPayload object.
   */
  virtual ~SetPressurePayload() override;

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual std::string serialize() override;

  /**
   * @brief Returns the pressures held by this payload.
   * @return The pressures held by this payload.
   */
  std::vector<double> getPressures() const;

  /**
   * @brief Get the unit of the pressures.
   * @return The unit of pressure.
   */
  PressureUnit getPressureUnit() const;

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
  std::vector<double> pressures;
  PressureUnit pressureUnit;
};

} // namespace Devices

#endif
