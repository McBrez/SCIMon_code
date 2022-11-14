#include <device_isx3.hpp>

namespace Devices {

DeviceIsx3::DeviceIsx3() : Device() {}

bool DeviceIsx3::configure(DeviceConfiguration *deviceConfiguration) {

}

bool DeviceIsx3::open() {}

bool DeviceIsx3::close() {}

bool DeviceIsx3::write(shared_ptr<InitDeviceMessage>) {

}

bool DeviceIsx3::write(shared_ptr<WriteDeviceMessage>) {}

shared_ptr<ReadDeviceMessage> DeviceIsx3::read() {

}

} // namespace Devices