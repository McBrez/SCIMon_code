#include <ack_message_isx3.hpp>

namespace Devices {

AckMessageIsx3::AckMessageIsx3(Isx3AckType ackType)
    : ReadDeviceMessage("ACK"), ackType(ackType) {}

string AckMessageIsx3::serialize() { return this->getData(); }

Isx3AckType AckMessageIsx3::getAckType() { return this->ackType; }

} // namespace Devices