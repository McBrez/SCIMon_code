#ifndef INIT_DEVICE_MESSAGE_HPP
#define INIT_DEVICE_MESSAGE_HPP

// Project includes
#include <device_message.hpp>

namespace Messages {

struct InitPayload {
  string deviceName;
  int channelOneConnfig;
  int channelTwoConnfig;
  int channelThreeConnfig;
  int channelFourConnfig;
};

/**
 * @brief Encapsulates a message that shall trigger a reset and initialization
 * of a device.
 */
class InitDeviceMessage : public DeviceMessage {
public:
  InitDeviceMessage();

  // TODO: remove me. and set the payload in the constructor.
  void setPayload(InitPayload *initPayload);
  shared_ptr<InitPayload> returnPayload();
  string serialize() override;

private:
  shared_ptr<InitPayload> payload;
};

} // namespace Messages
#endif