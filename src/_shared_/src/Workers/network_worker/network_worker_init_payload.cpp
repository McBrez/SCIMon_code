// Project includes
#include <common.hpp>
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

vector<unsigned char> NetworkWorkerInitPayload::bytes() {
  return vector<unsigned char>();
}

int NetworkWorkerInitPayload::getMagicNumber() {
  return MAGIC_NUMBER_NETWORK_WORKER_INIT_PAYLOAD;
}

} // namespace Workers