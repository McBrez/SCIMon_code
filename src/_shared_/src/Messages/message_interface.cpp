// Project includes
#include <message_interface.hpp>

namespace Messages {

MessageInterface::MessageInterface() : id(UserId()) {}

UserId MessageInterface::getUserId() const { return this->id; }
} // namespace Messages