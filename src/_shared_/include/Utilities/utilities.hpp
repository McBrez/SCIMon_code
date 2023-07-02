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

/**
 * @brief Splits a string at the given token and returns a std::list of strings.
 *
 * @param str The string that shall be split.
 * @param token The token at which shall be split.
 * @return The splitted strings.
 */
std::vector<std::string> split(const std::string &str, unsigned char token);

/**
 * @brief Joins a 3D array into a vector of impedance spectra.
 * @param array The array that shall be joined.
 * @param impedanceSpectrums Will contain the impedance spectra.
 * @param spectrumMapping A vector that defines the association between array
 * indices and frequency.
 */
void joinImpedanceSpectrum(
    const std::vector<std::vector<std::vector<double>>> &array,
    const std::vector<double> &spectrumMapping,
    std::vector<ImpedanceSpectrum> &impedanceSpectrums);

/**
 * @brief Joins the given frequencies and impedances to an impedance spectrum.
 * @param frequencies Vector containing frequencies.
 * @param impedances Vector containing the impedances.
 * @param isSpectrum Will contain the impedance spectrum.
 */
void joinImpedanceSpectrum(const std::vector<double> &frequencies,
                           const std::vector<Impedance> &impedances,
                           ImpedanceSpectrum &impedanceSpectrum);

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

/**
 * @brief splitImpedanceSpectrum
 * @param isSpectrum
 */
void splitImpedanceSpectrum(
    const std::vector<ImpedanceSpectrum> &isSpectrum,
    std::vector<std::vector<std::vector<double>>> &array);

/**
 * @brief Splits an impedance vector into a vector containing the real and
 * imaginary parts.
 * @return An impedance vector into a vector containing the real and
 * imaginary parts.
 */
void splitImpedance(const std::vector<Impedance> &impedanceVec,
                    std::vector<double> &realVec, std::vector<double> &imagVec);

} // namespace Utilities
#endif
