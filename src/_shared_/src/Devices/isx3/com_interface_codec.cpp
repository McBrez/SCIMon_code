// Project includes
#include <com_interface_codec.hpp>

namespace Devices {

ComInterfaceCodec::ComInterfaceCodec() {}

std::vector<unsigned char> ComInterfaceCodec::buildCmdXYZ() {
  return std::vector<unsigned char>();
}

std::vector<unsigned char> ComInterfaceCodec::buildCmdResetSystem() {
  return std::vector<unsigned char>(
      {CmdTagResetSystem, 0x00, CmdTagResetSystem});
}

std::vector<unsigned char> ComInterfaceCodec::buildCmdGetDeviceId() {
  return std::vector<unsigned char>(
      {CmdTagGetDeviceId, 0x00, CmdTagGetDeviceId});
}

std::vector<unsigned char>
ComInterfaceCodec::buildCmdInitMeasurement(bool readWrite,
                                           MeasurementMode measurementMode) {

  if (readWrite) {
    return std::vector<unsigned char>(
        {CmdTagInitMeasurement, 0x01, 0x80, CmdTagInitMeasurement});
  } else {
    return std::vector<unsigned char>(
        {CmdTagInitMeasurement, 0x02, 0x00,
         static_cast<unsigned char>(measurementMode), CmdTagInitMeasurement});
  }
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