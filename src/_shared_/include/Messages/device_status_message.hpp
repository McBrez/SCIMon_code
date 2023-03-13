#ifndef DEVICE_STATUS_MESSAGE
#define DEVICE_STATUS_MESSAGE

// Project inclues
#include <read_device_message.hpp>

namespace Messages {
/// Identifies the possible stati of a device.
enum DeviceStatus {
  UNKNOWN_DEVICE_STATUS,
  INIT,
  CONFIGURE,
  OPERATING,
  IDLE,
  BUSY

};
/**
 * @brief A message containing the status of the queried devices.
 *
 */
class DeviceStatusMessage : public ReadDeviceMessage {
public:
  DeviceStatusMessage(shared_ptr<MessageInterface> source,
                      ReadDeviceTopic topic, ReadPayload *readPayload,
                      shared_ptr<WriteDeviceMessage> originalMessage,
                      DeviceStatus deviceStatus,
                      string additionalInformation = "");

  DeviceStatus getDeviceStatus();

private:
  /// The device status.
  DeviceStatus deviceStatus;
};

} // namespace Messages

#endif
