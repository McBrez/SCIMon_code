// Standard includes
#include <chrono>
#include <functional>

// Project includes
#include <common.hpp>
#include <user_id.hpp>

using namespace Core;

namespace Messages {

UserId::UserId() : userId(0) {}

UserId::UserId(MessageInterface *messageInterface) {
  this->userId = this->generateUserId(messageInterface);
}

UserId::UserId(const UserId &other) { this->userId = other.id(); }

UserId::UserId(size_t userId) : userId(userId) {}

size_t UserId::id() const { return this->userId; }

size_t UserId::generateUserId(MessageInterface *messageInterface) {
  static int currentId = 0;
  currentId++;

  auto now = Core::getNow().time_since_epoch();
  long nowInt = now.count();
  long long pointerVal = (long long)messageInterface;

  return std::hash<int>{}(currentId) ^ std::hash<long>{}(nowInt) << 1 ^
         std::hash<long long>{}(pointerVal) << 2;
}

bool UserId::operator==(const UserId &other) {
  return this->userId == other.userId;
}

bool UserId::operator!=(const UserId &other) {
  return this->userId != other.userId;
}

UserId::operator bool() const { return this->isValid(); }

bool UserId::isValid() const { return this->userId != 0; }

bool UserId::operator<(const UserId &other) {
  return this->userId < other.userId;
}

} // namespace Messages

bool operator<(const Messages::UserId &lhs, const Messages::UserId &rhs) {
  return lhs.id() < rhs.id();
}
