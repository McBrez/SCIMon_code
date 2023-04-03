#ifndef INIT_DEVICE_MESSAGE_HPP
#define INIT_DEVICE_MESSAGE_HPP

// Project includes
#include <device_message.hpp>
#include <init_payload.hpp>
#include <user_id.hpp>

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
   * @param source Reference to the source of this message.
   * @param initPayload Pointer to the initialization data. The message takes
   * ownership of the pointer, hence the pointer has to stay valid.
   */
  InitDeviceMessage(shared_ptr<MessageInterface> source,
                    shared_ptr<MessageInterface> destination,
                    InitPayload *initPayload);

  InitDeviceMessage(shared_ptr<MessageInterface> source,
                    shared_ptr<MessageInterface> destination,
                    shared_ptr<InitPayload> initPayload);

  /**
   * @brief Destroy the Init Device Message object
   */
  virtual ~InitDeviceMessage() override;

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

private:
  /// Reference to the initialization data.
  shared_ptr<InitPayload> initPayload;
};

} // namespace Messages
#endif