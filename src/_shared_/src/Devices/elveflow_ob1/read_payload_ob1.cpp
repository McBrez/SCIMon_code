// Project includes
#include <common.hpp>
#include <read_payload_ob1.hpp>

namespace Devices {

ReadPayloadOb1::ReadPayloadOb1(Ob1ChannelPressures channelPressures)
    : channelPressures(channelPressures) {}

Ob1ChannelPressures ReadPayloadOb1::getChannelPressures() {
  return this->channelPressures;
}

std::string ReadPayloadOb1::serialize() {
  std::string retVal = "";
  retVal += "Pressure Channel 1: ";
  retVal += std::to_string(get<0>(this->channelPressures));
  retVal += "\n";
  retVal += "Pressure Channel 2: ";
  retVal += std::to_string(get<1>(this->channelPressures));
  retVal += "\n";
  retVal += "Pressure Channel 3: ";
  retVal += std::to_string(get<2>(this->channelPressures));
  retVal += "\n";
  retVal += "Pressure Channel 4: ";
  retVal += std::to_string(get<3>(this->channelPressures));
  return retVal;
}

std::vector<unsigned char> ReadPayloadOb1::bytes() {
  return std::vector<unsigned char>();
}

int ReadPayloadOb1::getMagicNumber() { return MAGIC_NUMBER_OB1_READ_PAYLOAD; }

} // namespace Devices