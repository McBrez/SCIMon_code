// Project includes
#include <id_payload.hpp>

namespace Devices {

IdPayload::IdPayload(string manufacturer, DeviceType deviceType,
                     int deviceIdentifier, int serialNumber)
    : ReadPayload(), manufacturer(manufacturer), deviceType(deviceType),
      deviceIdentifier(deviceIdentifier), serialNumber(serialNumber) {}

string IdPayload::serialize() { return ""; }

} // namespace Devices
