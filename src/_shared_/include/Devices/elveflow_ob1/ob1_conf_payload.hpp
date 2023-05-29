#ifndef OB1_CONF_PAYLOAD
#define OB1_CONF_PAYLOAD

#include <configuration_payload.hpp>

namespace Devices {
/**
 * @brief Encapsulates the configuration data of an OB1 device.
 */
class Ob1ConfPayload : public ConfigurationPayload {
public:
  /**
   * @brief Construct a new Ob 1 Conf Payload object
   */
  Ob1ConfPayload();

  /**
   * @brief Destroy the Ob 1 Conf Payload object
   */
  virtual ~Ob1ConfPayload() override;

  /**
   * @brief Returns the magic number of the payload. This number is used to
   * identify the payload type when encoding and decoding payload. This number
   * has to be unique among all payload types.
   */
  virtual int getMagicNumber() override;
};
} // namespace Devices

#endif