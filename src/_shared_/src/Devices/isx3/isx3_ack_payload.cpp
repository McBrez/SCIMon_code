// Project includes
#include <isx3_ack_payload.hpp>

namespace Devices {

Isx3AckPayload::Isx3AckPayload(Isx3AckType ackType) : ackType(ackType) {}

string Isx3AckPayload::serialize() { return ""; }

Isx3AckType Isx3AckPayload::getAckType() { return this->ackType; }

} // namespace Devices