// Project includes
#include <read_payload_ob1.hpp>

namespace Devices {

ReadPayloadOb1::ReadPayloadOb1(Ob1ChannelPressures channelPressures)
    : channelPressures(channelPressures) {}

Ob1ChannelPressures ReadPayloadOb1::getChannelPressures() {
  return this->channelPressures;
}

string ReadPayloadOb1::serialize() { return ""; }
} // namespace Devices