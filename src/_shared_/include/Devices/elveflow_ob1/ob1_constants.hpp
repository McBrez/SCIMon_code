#ifndef OB1_CONSTANTS_HPP
#define OB1_CONSTANTS_HPP

#include <string>

namespace Devices {
namespace Constants {

/// The name of the manufacturer of the OB1 device.
const std::string Ob1ManufacturerName = "Elveflow";
/// The names of the OB1 device.
const std::string Ob1DeviceTypeName = "OB1";
/// The length of the calibration array.
const unsigned int Ob1CalibrationArrayLen = 1000;

} // namespace Constants
} // namespace Devices
#endif