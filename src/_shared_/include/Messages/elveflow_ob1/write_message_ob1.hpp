#ifndef WRITE_MESSAGE_OB1_HPP
#define WRITE_MESSAGE_OB1_HPP

// Standard includes
#include <map>

// Project includes
#include <write_device_message.hpp>

using namespace std;

namespace Messages {

/**
 * @brief Identifies the topic of the OB1 message.
 */
enum Ob1Topic { OB1_TOPIC_INVALID, OB1_TOPIC_SET_PRESSURE };

/**
 * @brief Encapsulates a device-specific write message to an OB1 device.
 */
class WriteMessageOb1SetPressure : public WriteDeviceMessage {
public:
  WriteMessageOb1SetPressure(shared_ptr<MessageInterface> source,
                             shared_ptr<MessageInterface> destination,
                             map<int, double> setPressures);
  virtual string serialize() override;
  map<int, double> getSetPressure();
  Ob1Topic getOb1Topic();

private:
  Ob1Topic ob1Topic;
  /// Mapping from channel to set pressure in mBar.
  map<int, double> setPressures;
};
} // namespace Messages

#endif