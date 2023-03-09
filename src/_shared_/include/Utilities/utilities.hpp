#ifndef UTILITIES_HPP
#define UTILITIES_HPP

// Standard includes
#include <chrono>
#include <complex>
#include <list>
#include <string>
#include <vector>

using namespace std;

namespace Utilities {

// Shortcut to the definition of an impedance spectrum. The first value of the
// tuple is the frequency in Hertz. The second value is the complex impedance at
// this frequency.
typedef list<std::tuple<double, complex<double>>> ImpedanceSpectrum;

/// Defines a type that depcits a point in time.
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
/// Defines a time span.
using Duration = std::chrono::duration<int, std::milli>;

/**
 * @brief Splits a string at the given token and returns a list of strings.
 *
 * @param str The string that shall be split.
 * @param token The token at which shall be split.
 * @return The splitted strings.
 */
vector<string> split(const string &str, unsigned char token);
} // namespace Utilities
#endif