#include <init_message_isx3.hpp>

namespace Messages {
InitMessageIsx3::InitMessageIsx3(std::string telnetHost, int telnetPort)
    : InitDeviceMessage(), telnetHost(telnetHost), telnetPort(telnetPort) {}

std::string InitMessageIsx3::getTelnetHostName() { return this->telnetHost; }

int InitMessageIsx3::getTelnetPort() { return this->telnetPort; }

std::string InitMessageIsx3::serialize() {
  return std::string("InitMessageIsx3 telnetHost:" + this->telnetHost +
                     ", telnetPort:" + std::to_string(this->telnetPort));
}

} // namespace Messages
