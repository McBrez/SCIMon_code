// Project includes
#include <sentry_worker.hpp>

namespace Workers {
SentryWorker::SentryWorker() {}
SentryWorker::~SentryWorker() {}

void SentryWorker::work(TimePoint timestamp) {}

bool SentryWorker::specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) {
  return false;
}

list<shared_ptr<DeviceMessage>>
SentryWorker::specificRead(TimePoint timestamp) {
  return list<shared_ptr<DeviceMessage>>();
}

} // namespace Workers