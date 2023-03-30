// Project includes
#include <sentry_init_payload.hpp>

namespace Workers {

SentryInitPayload::SentryInitPayload(Isx3InitPayload *isx3InitPayload,
                                     Isx3IsConfPayload *isx3IsConfigPayload,
                                     Ob1InitPayload *ob1InitPayload,
                                     bool autoStart, Duration onTime,
                                     Duration offTime)
    : isx3InitPayload(isx3InitPayload),
      isx3IsConfigPayload(isx3IsConfigPayload), ob1InitPayload(ob1InitPayload),
      autoStart(autoStart), onTime(onTime), offTime(offTime) {}

string SentryInitPayload::serialize() { return ""; }
} // namespace Workers
