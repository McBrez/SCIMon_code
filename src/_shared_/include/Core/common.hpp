#ifndef COMMON_HPP
#define COMMON_HPP

// Standard includes
#include <chrono>
#include <complex>
#include <format>
#include <list>
#include <string>
#include <tuple>

namespace Core {

/// @brief Shortcut to a type that holds electric impedances.
typedef std::complex<double> Impedance;

/// Shortcut to the definition of an impedance at a given frequency. The first
/// value of the tuple is the frequency in Hertz. The second value is the
/// complex impedance in Ohm.
typedef std::tuple<double, Impedance> ImpedancePoint;

/// Shortcut to the definition of a discrete impedance spectrum.
typedef std::list<ImpedancePoint> ImpedanceSpectrum;

/// Defines a time span.
using Duration = std::chrono::duration<long long, std::milli>;

/// @brief shortcut to a type that enables timestamping.
using TimePoint = std::chrono::time_point<std::chrono::system_clock, Duration>;

/**
 * @brief Returns a timepoint object that represents the current time.
 * @return A timepoint object that represents the current time.
 */
TimePoint getNow();

/**
 * @brief Formats the given timepoint into a string.
 * @return String representing the given timepoint.
 */
std::string getTimestampString(TimePoint timepoints);

const std::string NETWORK_WORKER_TYPE_NAME = "Network Worker";
const std::string OB1_DEVICE_TYPE_NAME = "Elveflow OB1";
const std::string ISX3_DEVICE_TYPE_NAME = "Sciospec ISX3";
const std::string EPIX_DEVICE_TYPE_NAME = "Sciospec EPIX";
const std::string SENTRY_WORKER_TYPE_NAME = "Sentry Worker";
const std::string CONTROL_WORKER_TYPE_NAME = "Control Worker";

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
#define MAGIC_NUMBER_REQUEST_DATA_PAYLOAD 0x0009
#define MAGIC_NUMBER_SET_DEVICE_STATUS_PAYLOAD 0x000A
#define MAGIC_NUMBER_DATA_RESPONSE_PAYLOAD 0x000B
#define MAGIC_NUMBER_REQUEST_KEY_PAYLOAD 0x000C
#define MAGIC_NUMBER_KEY_RESPONSE_PAYLOAD 0x000D
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

/**
 * @brief Overloaded lesser than operator for impedances
 * @param lhs The left hand side.
 * @param rhs The right hand side.
 * @return True if the absolute value of the left hand side is smaller than the
 * right hand side.
 */
bool operator<(const Core::Impedance &lhs, const Core::Impedance &rhs);

/**
 * @brief Overloaded greater than operator for impedances
 * @param lhs The left hand side.
 * @param rhs The right hand side.
 * @return True if the absolute value of the left hand side is greater than the
 * right hand side.
 */
bool operator>(const Core::Impedance &lhs, const Core::Impedance &rhs);

#endif
