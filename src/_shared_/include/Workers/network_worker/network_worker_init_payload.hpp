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
  /// Network worker is std::listening and waiting for communication.
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
  /**
   * @brief Construct a new Network Worker Init Payload object
   *
   * @param operationMode The operation mode the targeted network worker shall
   * initialize into.
   * @param ipAddress If operation mode is NETWORK_WORKER_OP_MODE_CLIENT, this
   * is the ip that shall be connected to.
   * @param port Depending on the operation mode, this is the port that is
   * connected, or std::listened to.
   */
  NetworkWorkerInitPayload(NetworkWorkerOperationMode operationMode,
                           std::string ipAddress, int port);

  /**
   * @brief Destroy the Network Worker Init Payload objectF
   */
  ~NetworkWorkerInitPayload();

  /**
   * @brief Returns the operation mode.
   * @return The operation mode held by this object.
   */
  NetworkWorkerOperationMode getOperationMode();

  /**
   * @brief Returns the ip address held by this object.
   * @return The ip address held by this object.
   */
  std::string getIpAddress();

  /**
   * @brief Returns the port held by this object.
   * @return The port held by this object.
   */
  int getPort();

  /**
   * @brief  Serializes the object.
   * @return String representation of the object.
   */
  virtual std::string serialize() override;

  /**
   * @brief Serializes the payload into bytes.
   * @return Byte vector, that depicts the payload.
   */
  virtual std::vector<unsigned char> bytes() override;

  /**
   * @brief Returns the magic number of the payload. This number is used to
   * identify the payload type when encoding and decoding payload. This number
   * has to be unique among all payload types.
   */
  virtual int getMagicNumber() override;

private:
  /// The operation mode that the network worker should initializie into.
  NetworkWorkerOperationMode operationMode;
  /// The ip address that the worker shall connect to, when in
  /// NETWORK_WORKER_OP_MODE_CLIENT mode.
  std::string ipAddress;
  /// The port that shall be std::listened to or connected to (depending on the
  /// operation mode).
  int port;
};
} // namespace Workers

#endif