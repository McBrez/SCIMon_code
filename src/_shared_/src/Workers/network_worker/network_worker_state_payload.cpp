// Project includes
#include <network_worker_state_payload.hpp>

using namespace Workers;

NetworkWorkerStatePayload::NetworkWorkerStatePayload(
    NetworkWorkerCommState networkWorkerState)
    : networkWorkerState(networkWorkerState) {}

std::vector<unsigned char> NetworkWorkerStatePayload::bytes() {
  // Not yet implemented.
  return std::vector<unsigned char>();
}

int NetworkWorkerStatePayload::getMagicNumber() {
  // Not yet implemented.
  return -1;
}

NetworkWorkerCommState
NetworkWorkerStatePayload::getNetworkWorkerState() const {
  return this->networkWorkerState;
}

std::string NetworkWorkerStatePayload::serialize() { return ""; }
