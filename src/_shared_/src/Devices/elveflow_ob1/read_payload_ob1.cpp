// Project includes
#include <read_payload_ob1.hpp>

namespace Devices {

ReadPayloadOb1::ReadPayloadOb1(Ob1ChannelPressures channelPressures)
    : channelPressures(channelPressures) {}

Ob1ChannelPressures ReadPayloadOb1::getChannelPressures() {
  return this->channelPressures;
}

string ReadPayloadOb1::serialize() {
  string retVal = "";
  retVal += "Pressure Channel 1: ";
  retVal += to_string(get<0>(this->channelPressures));
  retVal += "\n";
  retVal += "Pressure Channel 2: ";
  retVal += to_string(get<1>(this->channelPressures));
  retVal += "\n";
  retVal += "Pressure Channel 3: ";
  retVal += to_string(get<2>(this->channelPressures));
  retVal += "\n";
  retVal += "Pressure Channel 4: ";
  retVal += to_string(get<3>(this->channelPressures));
  return retVal;
}

vector<unsigned char> ReadPayloadOb1::bytes() {
  return vector<unsigned char>();
}

} // namespace Devices