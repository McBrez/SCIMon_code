// Project includes
#include <network_worker.hpp>

namespace Workers {
NetworkWorker::NetworkWorker() : Worker() {}

NetworkWorker::~NetworkWorker() {}

void NetworkWorker::work(TimePoint timestamp) {}

bool NetworkWorker::start() { return false; }

bool NetworkWorker::stop() { return false; }

bool NetworkWorker::initialize(shared_ptr<InitPayload> initPayload) {
  return false;
}

bool NetworkWorker::configure(shared_ptr<ConfigurationPayload> configPayload) {
  return false;
}

bool NetworkWorker::specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) {
  return false;
}

list<shared_ptr<DeviceMessage>>
NetworkWorker::specificRead(TimePoint timestamp) {
  return list<shared_ptr<DeviceMessage>>();
}

bool NetworkWorker::handleResponse(shared_ptr<ReadDeviceMessage> response) {
  return false;
}

} // namespace Workers