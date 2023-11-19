// Standard includes
#include <format>

// Project includes
#include <common.hpp>

Core::TimePoint Core::getNow() {
  return Core::TimePoint(std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()));
}

Core::TimePoint Core::getTimeFromStr(const std::string &dateString,
                                     const std::string &formatString) {
  std::istringstream ss(dateString);
  Core::TimePoint timestamp;
  ss >> std::chrono::parse(formatString, timestamp);

  return timestamp;
}

std::string Core::getTimestampString(TimePoint timepoints) {
  return std::format("{:%FT%T%z}", timepoints);
}

bool operator<(const Core::Impedance &lhs, const Core::Impedance &rhs) {
  return (lhs.real() * lhs.real() + lhs.imag() * lhs.imag()) <
         (rhs.real() * rhs.real() + rhs.imag() * rhs.imag());
}

bool operator>(const Core::Impedance &lhs, const Core::Impedance &rhs) {
  return (lhs.real() * lhs.real() + lhs.imag() * lhs.imag()) >
         (rhs.real() * rhs.real() + rhs.imag() * rhs.imag());
}
