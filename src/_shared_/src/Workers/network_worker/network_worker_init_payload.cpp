// Project includes
#include <network_worker_init_payload.hpp>

namespace Workers {

NetworkWorkerInitPayload::NetworkWorkerInitPayload(
    NetworkWorkerOperationMode operationMode, string ipAddress, int port)
    : operationMode(operationMode), ipAddress(ipAddress), port(port) {}

NetworkWorkerInitPayload::~NetworkWorkerInitPayload() {}

NetworkWorkerOperationMode NetworkWorkerInitPayload::getOperationMode() {
  return this->operationMode;
}

string NetworkWorkerInitPayload::getIpAddress() { return this->ipAddress; }

int NetworkWorkerInitPayload::getPort() { return this->port; }

string NetworkWorkerInitPayload::serialize() { return ""; }

} // namespace Workers