#include <message_factory.hpp>

namespace Messages {

shared_ptr<WriteDeviceMessage> MessageFactory::createRunMessage() {
  return shared_ptr<WriteDeviceMessage>();
}
} // namespace Messages