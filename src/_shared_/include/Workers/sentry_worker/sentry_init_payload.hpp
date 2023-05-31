#ifndef SENTRY_WORKER_PAYLOAD
#define SENTRY_WORKER_PAYLOAD

// Standard includes
#include <memory>
#include <string>

// Project includes
#include <configuration_payload.hpp>
#include <init_payload.hpp>
#include <utilities.hpp>

using namespace std;
using namespace Devices;
using namespace Utilities;

namespace Workers {
class SentryInitPayload : public InitPayload {
public:
  SentryInitPayload(InitPayload *isInitPayload,
                    ConfigurationPayload *isConfigPayload,
                    InitPayload *pumpControllerInitPayload,
                    ConfigurationPayload *pumpControllerConfigPayload);

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual string serialize() override;

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

  // Impedance spectrometer init data
  const shared_ptr<InitPayload> isSpecInitPayload;
  // Impedance spectrometer config data
  const shared_ptr<ConfigurationPayload> isSpecConfPayload;

  // Pump controller init data.
  const shared_ptr<InitPayload> pumpControllerInitPayload;
  // Pump controller config data
  const shared_ptr<ConfigurationPayload> pumpControllerConfigPayload;
};
} // namespace Workers

#endif