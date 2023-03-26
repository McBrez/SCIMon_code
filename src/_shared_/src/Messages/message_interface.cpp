// Project includes
#include <message_interface.hpp>

namespace Messages {

MessageInterface::MessageInterface()
    : id(UserId()), messageDistributor(nullptr) {}

UserId MessageInterface::getUserId() const { return this->id; }

bool MessageInterface::operator==(MessageInterface &other) {
  return this->getUserId() == other.getUserId();
}
} // namespace Messages