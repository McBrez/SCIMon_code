#ifndef ACK_MESSAGE_ISX3_HPP
#define ACK_MESSAGE_ISX3_HPP

// Projects includes
#include <isx3_constants.hpp>
#include <read_device_message.hpp>

namespace Devices {

class AckMessageIsx3 : public ReadDeviceMessage {
public:
  AckMessageIsx3(Isx3AckType ackType);

  virtual string serialize() override;

  Isx3AckType getAckType();

private:
  Isx3AckType ackType;
};
} // namespace Devices

#endif