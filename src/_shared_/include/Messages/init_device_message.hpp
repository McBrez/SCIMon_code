#ifndef INIT_DEVICE_MESSAGE_HPP
#define INIT_DEVICE_MESSAGE_HPP

// Project includes
#include <device_id.hpp>
#include <device_message.hpp>
#include <init_payload.hpp>

using namespace Devices;

namespace Messages {

/**
 * @brief Encapsulates a message that shall trigger a reset and initialization
 * of a device.
 */
class InitDeviceMessage : public DeviceMessage {
public:
  /**
   * @brief Construct a new Init Device Message object
   * @param initPayload Pointer to the initialization data. The message takes
   * ownership of the pointer, hence the pointer has to stay valid.
   * @param targetDeviceId The id of the target device.
   */
  InitDeviceMessage(InitPayload *initPayload, DeviceId targetDeviceId);

  /**
   * @brief Returns the initialization data.
   * @return The initialization data.
   */
  shared_ptr<InitPayload> returnPayload();

  /**
   * @brief Serializes the message into a human-readable string.
   * @return A human-readable representation of the message.
   */
  string serialize() override;

  /**
   * @brief Returns the device id of the targeted device.
   * @return The device id.
   */
  DeviceId getTargetDeviceId();

private:
  /// Reference to the initialization data.
  shared_ptr<InitPayload> initPayload;

  /// The id of the target device.
  DeviceId targetDeviceId;
};

} // namespace Messages
#endif