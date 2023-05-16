// Standard includes
#include <chrono>
#include <functional>

// Project includes
#include <user_id.hpp>

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

  auto now = std::chrono::system_clock::now().time_since_epoch();
  long nowInt = now.count();
  long long pointerVal = (long long)messageInterface;

  return std::hash<int>{}(currentId) ^ std::hash<long>{}(nowInt) << 1 ^
         std::hash<long long>{}(pointerVal) << 2;
}

bool UserId::operator==(const UserId &other) {
  return this->userId == other.id();
}

bool UserId::operator!=(const UserId &other) {
  return this->userId != other.id();
}

UserId::operator bool() const { return this->isValid(); }

bool UserId::isValid() const { return this->userId != 0; }

} // namespace Messages