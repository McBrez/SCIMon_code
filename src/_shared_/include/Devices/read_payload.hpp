#ifndef READ_PAYLOAD_HPP
#define READ_PAYLOAD_HPP

// Standard includes
#include <string>

using namespace std;

namespace Devices {

/**
 * @brief Base class for a read data package.
 */
class ReadPayload {
public:
  /**
   * @brief Construct a new ReadPayload object.
   */
  ReadPayload();

  /**
   * @brief Destroy the ReadPayload object.
   */
  virtual ~ReadPayload() = 0;

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual string serialize() = 0;

  /**
   * @brief Returns the additional data held by this object.
   * @return The additional data string.
   */
  string getAdditionalData() const;

  /**
   * @brief Sets the additional data field of this object.
   * @param additionalData The additional data that shall be set.
   */
  void setAdditionalData(const string &additionalData);

private:
  /// String for uncritical additional data.
  string additionalData;
};

} // namespace Devices

#endif