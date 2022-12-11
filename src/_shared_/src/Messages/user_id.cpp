// Standard includes
#include <climits>

// Project includes
#include <user_id.hpp>

namespace Messages {

UserId::UserId() { this->userId = this->generateUserId(); }

UserId::UserId(int userId) : userId(userId) {}

int UserId::id() const { return this->userId; }

int UserId::generateUserId() {
  static int currentId = 0;

  if (currentId == INT_MAX) {
    return -1;
  } else {
    return currentId++;
  }
}

bool UserId::operator==(const UserId &other) {
  return this->userId == other.id();
}

bool UserId::operator!=(const UserId &other) {
  return this->userId != other.id();
}

} // namespace Messages