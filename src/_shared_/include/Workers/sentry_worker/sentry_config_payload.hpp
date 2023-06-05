#ifndef SENTRY_WORKER_CONFIG_PAYLOAD
#define SENTRY_WORKER_CONFIG_PAYLOAD

// Project includes
#include <configuration_payload.hpp>
#include <utilities.hpp>

using namespace Devices;
using namespace std;
using namespace Utilities;

namespace Workers {

/**
 * @brief Identifies the worker modes of the sentry workers.
 */
enum SentryWorkerMode {
  /// @brief Invalid worker mode.
  SENTRY_WORKER_MODE_INVALID = 0,
  /// @brief Worker is controlled manually via write messages.
  SENTRY_WORKER_MODE_MANUAL = 1,
  /// @brief Worker is running automatically on a timer.
  SENTRY_WORKER_MODE_TIMER = 2
};

class SentryConfigPayload : public ConfigurationPayload {
public:
  /**
   * @brief Construct a new Sentry Config Payload object
   *
   * @param sentryWorkerMode The mode in which the sentry worker shall operate.
   * @param onTime In case timer mode is selected by sentryWorkerMode, this
   * contains the time the sentry shall turn on the pump.
   * @param offTime In case timer mode is selected by sentryWorkerMode, this
   * contains the time the sentry shall turn off the pump.  
   */
  SentryConfigPayload(SentryWorkerMode sentryWorkerMode, Duration onTime,
                      Duration offTime);

  /**
   * @brief Serializes the payload into bytes.
   * @return Byte vector, that depicts the payload.
   */
  virtual vector<unsigned char> bytes() override;

  /**
   * @brief Returns the magic number of the payload. This number is used to
   * identify the payload type when encoding and decoding payload. This number
   * has to be unique among all payload types.
   */
  virtual int getMagicNumber() override;

  SentryWorkerMode sentryWorkerMode;
  Duration onTime;
  Duration offTime;
};
} // namespace Workers

#endif