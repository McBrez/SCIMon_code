#ifndef INIT_MESSAGE_ISX3_HPP
#define INIT_MESSAGE_ISX3_HPP

// Standard includes
#include <string>

// Project includes
#include <init_device_message.hpp>

namespace Messages {
class InitMessageIsx3 : public InitDeviceMessage {
public:
  InitMessageIsx3(std::string telnetHost, int telnetPort);

  std::string getTelnetHostName();

  /**
   * Returns the telnet port, the device shall be initialized with.
   *
   * @return The telnet port, the device shall be initialized with.
   */
  int getTelnetPort();

  /**
   * @brief Serializes the message into a string.
   * @return A string containing the contents of the message.
   */
  virtual string serialize() override;

private:
  /// The name or the ip of the telnet host.
  std::string telnetHost;
  /// The port of the telnet host.
  int telnetPort;
};

} // namespace Messages

#endif