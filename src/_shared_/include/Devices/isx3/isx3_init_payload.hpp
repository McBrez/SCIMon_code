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
   * @param ipAddress The ip address that shall be connected to.
   * @param port The port that shall be connected to.
   */
  Isx3InitPayload(std::string ipAddress, int port);

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual std::string serialize() override;

  std::string getIpAddress();

  int getPort();

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
  std::string ipAddress;
  int port;
};
} // namespace Devices

#endif