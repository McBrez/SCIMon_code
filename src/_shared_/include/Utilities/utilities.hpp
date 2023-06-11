#ifndef UTILITIES_HPP
#define UTILITIES_HPP

// Standard includes
#include <chrono>
#include <complex>
#include <list>
#include <string>
#include <vector>

// Project includes.
#include <common.hpp>

using namespace Core;

namespace Utilities {

/// Defines a type that depcits a point in time.
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
/// Defines a time span.
using Duration = std::chrono::duration<int, std::milli>;

/**
 * @brief Splits a string at the given token and returns a std::list of strings.
 *
 * @param str The string that shall be split.
 * @param token The token at which shall be split.
 * @return The splitted strings.
 */
std::vector<std::string> split(const std::string &str, unsigned char token);

/**
 * @brief Splits the impedance spectrum into two separate vectors containing the
 * frequencies and impedances respectivelly.
 *
 * @param isSpectrum The Impedance spectrum that shall be split.
 * @param frequencies A vector containing the frequencies of the spectrum.
 * @param impedance A vector containing the impedances of the spectrum.
 */
void splitImpedanceSpectrum(const ImpedanceSpectrum &isSpectrum,
                            std::vector<double> &frequencies,
                            std::vector<Impedance> &impedance);
} // namespace Utilities
#endif