#ifndef ISX3_INIT_PAYLOAD
#define ISX3_INIT_PAYLOAD

// Standard includes
#include <string>

// Project includes
#include <init_payload.hpp>

using namespace std;

namespace Devices {
class Isx3InitPayload : public InitPayload {
public:
  /**
   * @brief Construct a new Isx 3 Init Payload object
   * @param ipAddress The ip address that shall be connected to.
   * @param port The port that shall be connected to.
   */
  Isx3InitPayload(string ipAddress, int port);

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual string serialize() override;

  string getIpAddress();

  int getPort();

  /**
   * @brief Serializes the payload into bytes.
   * @return Byte vector, that depicts the payload.
   */
  virtual vector<unsigned char> bytes() override;

private:
  string ipAddress;
  int port;
};
} // namespace Devices

#endif