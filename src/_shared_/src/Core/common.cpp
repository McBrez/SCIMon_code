// Standard includes
#include <format>

// Project includes
#include <common.hpp>

Core::TimePoint Core::getNow() {
  return Core::TimePoint(std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()));
}

std::string Core::getTimestampString(TimePoint timepoints) {
  return std::format("{:%FT%T%z}", timepoints);
}
