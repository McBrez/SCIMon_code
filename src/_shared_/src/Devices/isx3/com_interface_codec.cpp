// Project includes
#include <com_interface_codec.hpp>

namespace Devices {

ComInterfaceCodec::ComInterfaceCodec() {}

std::vector<unsigned char> ComInterfaceCodec::buildCmdXYZ() {
  return std::vector<unsigned char>();
}

shared_ptr<DeviceMessage>
ComInterfaceCodec::decodeMessage(std::vector<unsigned char> bytes) {
  return shared_ptr<DeviceMessage>();
}

std::vector<unsigned char>
ComInterfaceCodec::encodeMessage(shared_ptr<DeviceMessage> message) {
  return std::vector<unsigned char>();
}
} // namespace Devices