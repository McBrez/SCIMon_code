// Project includes
#include <common.hpp>
#include <isx3_ack_payload.hpp>

namespace Devices {

Isx3AckPayload::Isx3AckPayload(Isx3AckType ackType, Isx3CmdTag cmdTag)
    : ackType(ackType), cmdTag(cmdTag) {}

std::string Isx3AckPayload::serialize() { return ""; }

Isx3AckType Isx3AckPayload::getAckType() { return this->ackType; }

Isx3CmdTag Isx3AckPayload::getCmdTag() { return this->cmdTag; }

std::vector<unsigned char> Isx3AckPayload::bytes() {
  return std::vector<unsigned char>();
}

int Isx3AckPayload::getMagicNumber() { return MAGIC_NUMBER_ISX3_ACK_PAYLOAD; }

} // namespace Devices