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

/// @brief The default port for network worker connections.
const int NetworkWorkerDefaultPort = 4545;

// ----------------------------------------------- Magic numbers for payloads --
// Built-in payloads
#define MAGIC_NUMBER_GENERIC_PAYLOAD 0x0001
#define MAGIC_NUMBER_STATUS_PAYLOAD 0x0002
#define MAGIC_NUMBER_IS_PAYLOAD 0x0003
#define MAGIC_NUMBER_IS_CONFIGURATION 0x0004
#define MAGIC_NUMBER_ID_PAYLOAD 0x0005
#define MAGIC_NUMBER_GENERIC_READ_PAYLOAD 0x0006
#define MAGIC_NUMBER_NETWORK_WORKER_INIT_PAYLOAD 0x0007
#define MAGIC_NUMBER_SET_PRESSURE_PAYLOAD 0x0008
// ISX3 Payloads
#define MAGIC_NUMBER_ISX3_INIT_PAYLOAD 0x0101
#define MAGIC_NUMBER_ISX3_IS_CONF_PAYLOAD 0x0102
#define MAGIC_NUMBER_ISX3_ACK_PAYLOAD 0x0103
// OB1 Payloads
#define MAGIC_NUMBER_OB1_CONF_PAYLOAD 0x0201
#define MAGIC_NUMBER_OB1_INIT_PAYLOAD 0x0202
#define MAGIC_NUMBER_OB1_READ_PAYLOAD 0x0203
#define MAGIC_NUMBER_OB1_WRITE_PAYLOAD 0x0204
// Sentry payloads
#define MAGIC_NUMBER_SENTRY_INIT_PAYLOAD 0x0301
#define MAGIC_NUMBER_SENTRY_CONF_PAYLOAD 0x0302
} // namespace Core
#endif