#ifndef COMMON_HPP
#define COMMON_HPP

// Standard includes
#include <chrono>
#include <complex>
#include <list>
#include <tuple>

using namespace std;

namespace Core {

/// Shortcut to the definition of an impedance at a given frequency. The first
/// value of the tuple is the frequency in Hertz. The second value is the
/// complex impedance in Ohm.
typedef tuple<double, complex<double>> ImpedancePoint;

// Shortcut to the definition of a discrete impedance spectrum.
typedef list<ImpedancePoint> ImpedanceSpectrum;

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
} // namespace Core
#endif