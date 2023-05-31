#ifndef COMMON_HPP
#define COMMON_HPP

// Standard includes
#include <chrono>
#include <complex>
#include <list>
#include <string>
#include <tuple>

using namespace std;

namespace Core {

/// @brief Shortcut to a type that holds electric impedances.
typedef complex<double> Impedance;

/// Shortcut to the definition of an impedance at a given frequency. The first
/// value of the tuple is the frequency in Hertz. The second value is the
/// complex impedance in Ohm.
typedef tuple<double, Impedance> ImpedancePoint;

/// Shortcut to the definition of a discrete impedance spectrum.
typedef list<ImpedancePoint> ImpedanceSpectrum;

/// @brief shortcut to a type that enables timestamping.
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

const string NETWORK_WORKER_TYPE_NAME = "Network Worker";

const string SENTRY_WORKER_TYPE_NAME = "Sentry Worker";

// ----------------------------------------------- Magic numbers for payloads --
#define MAGIC_NUMBER_GENERIC_PAYLOAD 0x0001
#define MAGIC_NUMBER_STATUS_PAYLOAD 0x0002
#define MAGIC_NUMBER_IS_PAYLOAD 0x0003
#define MAGIC_NUMBER_IS_CONFIGURATION 0x0004
#define MAGIC_NUMBER_ID_PAYLOAD 0x0005
#define MAGIC_NUMBER_GENERIC_READ_PAYLOAD 0x0006
#define MAGIC_NUMBER_NETWORK_WORKER_INIT_PAYLOAD 0x0007
#define MAGIC_NUMBER_SET_PRESSURE_PAYLOAD 0x0008
#define MAGIC_NUMBER_ISX3_INIT_PAYLOAD 0x0107
#define MAGIC_NUMBER_ISX3_IS_CONF_PAYLOAD 0x0108
#define MAGIC_NUMBER_ISX3_ACK_PAYLOAD 0x0109
#define MAGIC_NUMBER_OB1_CONF_PAYLOAD 0x020A
#define MAGIC_NUMBER_OB1_INIT_PAYLOAD 0x020B
#define MAGIC_NUMBER_OB1_READ_PAYLOAD 0x020C

} // namespace Core
#endif