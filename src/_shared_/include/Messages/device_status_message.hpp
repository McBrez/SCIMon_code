#ifndef DEVICE_STATUS_MESSAGE
#define DEVICE_STATUS_MESSAGE

// Project inclues
#include <read_device_message.hpp>
#include <status_payload.hpp>

using namespace Devices;

namespace Messages {

/**
 * @brief A message containing the status of the queried devices.
 */
class DeviceStatusMessage : public ReadDeviceMessage {
public:
  DeviceStatusMessage(shared_ptr<MessageInterface> source,
                      shared_ptr<MessageInterface> destination,
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
