#ifndef ISX3_INIT_PAYLOAD
#define ISX3_INIT_PAYLOAD

// Standard includes
#include <string>

// Project includes
#include <init_payload.hpp>

namespace Devices {
class Isx3InitPayload : public InitPayload {
public:
  /**
   * @brief Construct a new Isx 3 Init Payload object
   * @param comPort The COM port the device shall connect to.
   */
  Isx3InitPayload(int comPort);

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual std::string serialize() override;

  int getComPort() const;

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
  int getMagicNumber() override;

private:
  int comPort;
};
} // namespace Devices

#endif
