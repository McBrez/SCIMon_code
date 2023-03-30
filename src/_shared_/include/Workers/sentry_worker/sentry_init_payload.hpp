#ifndef SENTRY_WORKER_PAYLOAD
#define SENTRY_WORKER_PAYLOAD

// Standard includes
#include <memory>
#include <string>

// Project includes
#include <init_payload.hpp>
#include <isx3_init_payload.hpp>
#include <isx3_is_conf_payload.hpp>
#include <ob1_init_payload.hpp>
#include <utilities.hpp>

using namespace std;
using namespace Devices;
using namespace Utilities;

namespace Workers {
class SentryInitPayload : public Devices::InitPayload {
public:
  SentryInitPayload(Isx3InitPayload *isx3InitPayload,
                    Isx3IsConfPayload *isx3IsConfigPayload,
                    Ob1InitPayload *ob1InitPayload, bool autoStart,
                    Duration onTime, Duration offTime);

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual string serialize() override;

  // ISX3 Init data
  const shared_ptr<Isx3InitPayload> isx3InitPayload;

  // ISX3 Config data
  const shared_ptr<Isx3IsConfPayload> isx3IsConfigPayload;

  // OB1 Init data
  const shared_ptr<Ob1InitPayload> ob1InitPayload;

  // etc
  const bool autoStart;
  Duration onTime;
  Duration offTime;
};
} // namespace Workers

#endif