#ifndef CONFIG_DEVICE_MESSAGE_HPP
#define CONFIG_DEVICE_MESSAGE_HPP

#include <write_device_message.hpp>

namespace Messages {
class ConfigDeviceMessage : public WriteDeviceMessage {
  virtual string serialize() override;
};
} // namespace Messages

#endif