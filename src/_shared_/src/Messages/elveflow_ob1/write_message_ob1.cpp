// Project includes
#include <write_message_ob1.hpp>

using namespace std;

namespace Messages {

WriteMessageOb1SetPressure::WriteMessageOb1SetPressure(
    shared_ptr<MessageInterface> source,
    shared_ptr<MessageInterface> destination, map<int, double> setPressures)
    : WriteDeviceMessage(source, destination,
                         WriteDeviceTopic::WRITE_TOPIC_DEVICE_SPECIFIC),
      setPressures(setPressures), ob1Topic(Ob1Topic::OB1_TOPIC_SET_PRESSURE) {}

string WriteMessageOb1SetPressure::serialize() { return ""; }

map<int, double> WriteMessageOb1SetPressure::getSetPressure() {
  return this->setPressures;
}

Ob1Topic WriteMessageOb1SetPressure::getOb1Topic() { return this->ob1Topic; }

} // namespace Messages
