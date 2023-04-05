#ifndef NETWORK_WORKER_INIT_PAYLOAD_HPP
#define NETWORK_WORKER_INIT_PAYLOAD_HPP

// Project includes
#include <init_payload.hpp>

using namespace Devices;

namespace Workers {

/// Identifies the operation modes of the network worker.
enum NetworkWorkerOperationMode {
  /// Invalid operation mode.
  NETWORK_WORKER_OP_MODE_INVALID,
  /// Network worker is listening and waiting for communication.
  NETWORK_WORKER_OP_MODE_SERVER,
  /// Network worker is connecting to a server and sends it instructions.
  NETWORK_WORKER_OP_MODE_CLIENT
};

/**
 * @brief Encapsulates the information needed for the initialization of a
 * network worker.
 */
class NetworkWorkerInitPayload : public InitPayload {
public:
  NetworkWorkerInitPayload(NetworkWorkerOperationMode operationMode,
                           string ipAddress, int port);

  ~NetworkWorkerInitPayload();

  NetworkWorkerOperationMode getOperationMode();

  string getIpAddress();

  int getPort();

  virtual string serialize() override;

private:
  NetworkWorkerOperationMode operationMode;
  string ipAddress;
  int port;
};
} // namespace Workers

#endif