#ifndef DUMMY_OBJECT_HPP
#define DUMMY_OBJECT_HPP

// Project includes
#include <write_device_message.hpp>

namespace Messages {

class DummyMessage : public WriteDeviceMessage {
public:
  DummyMessage();

  /**
   * @brief Serializes the message into a string.
   * @return A string containing the contents of the message.
   */
  virtual string serialize() override;
};

} // namespace Messages

#endif