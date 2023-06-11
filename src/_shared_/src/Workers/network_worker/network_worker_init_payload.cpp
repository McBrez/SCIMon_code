// Project includes
#include <common.hpp>
#include <network_worker_init_payload.hpp>

namespace Workers {

NetworkWorkerInitPayload::NetworkWorkerInitPayload(
    NetworkWorkerOperationMode operationMode, std::string ipAddress, int port)
    : operationMode(operationMode), ipAddress(ipAddress), port(port) {}

NetworkWorkerInitPayload::~NetworkWorkerInitPayload() {}

NetworkWorkerOperationMode NetworkWorkerInitPayload::getOperationMode() {
  return this->operationMode;
}

std::string NetworkWorkerInitPayload::getIpAddress() { return this->ipAddress; }

int NetworkWorkerInitPayload::getPort() { return this->port; }

std::string NetworkWorkerInitPayload::serialize() { return ""; }

std::vector<unsigned char> NetworkWorkerInitPayload::bytes() {
  return std::vector<unsigned char>();
}

int NetworkWorkerInitPayload::getMagicNumber() {
  return MAGIC_NUMBER_NETWORK_WORKER_INIT_PAYLOAD;
}

} // namespace Workers