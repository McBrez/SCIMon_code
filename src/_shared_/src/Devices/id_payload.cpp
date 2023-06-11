// Project includes
#include <common.hpp>
#include <id_payload.hpp>

namespace Devices {

IdPayload::IdPayload(std::string manufacturer, DeviceType deviceType,
                     int deviceIdentifier, int serialNumber)
    : ReadPayload(), manufacturer(manufacturer), deviceType(deviceType),
      deviceIdentifier(deviceIdentifier), serialNumber(serialNumber) {}

std::string IdPayload::serialize() { return ""; }

std::vector<unsigned char> IdPayload::bytes() {
  return std::vector<unsigned char>();
}

int IdPayload::getMagicNumber() { return MAGIC_NUMBER_ID_PAYLOAD; }

} // namespace Devices
