// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <worker.hpp>

namespace Workers {

WorkerId Worker::workerIdCounter = 0;

Worker::Worker(unsigned int dataManagerMeasurementLevel,
               Utilities::DataManagerType dataManagerType)
    : MessageInterface(DeviceType::UNSPECIFIED, dataManagerMeasurementLevel,
                       dataManagerType),
      workerId(++Worker::workerIdCounter) {}

Worker::~Worker() {}

std::shared_ptr<StatusPayload> Worker::constructStatus() {
  return std::shared_ptr<StatusPayload>(new StatusPayload(
      this->getUserId(), this->deviceState, this->getProxyUserIds(),
      DeviceType::UNSPECIFIED, this->getWorkerName(), this->initPayload,
      this->configPayload));
}

} // namespace Workers
