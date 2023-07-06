// Project includes
#include <common.hpp>

Core::TimePoint Core::getNow() {
  return Core::TimePoint(std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()));
}
