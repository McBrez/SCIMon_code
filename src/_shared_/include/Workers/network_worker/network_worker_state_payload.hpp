#ifndef NETWORK_WORKER_STATE_PAYLOAD_HPP
#define NETWORK_WORKER_STATE_PAYLOAD_HPP

// Project includes
#include <network_worker.hpp>
#include <read_payload.hpp>

using namespace Devices;

namespace Workers {
/**
 * @brief A payload that holds the state of an network worker. This payload is
 * usually sent, when a network worker lost the connection to it's counterpart.
 */
class NetworkWorkerStatePayload : public ReadPayload {
public:
  /**
   * @brief Creates the Object.
   * @param networkWorkerState The state that shall be held by this payload.
   */
  NetworkWorkerStatePayload(NetworkWorkerCommState networkWorkerState);

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

  /**
   * @brief Returns the state of the network worker.
   * @return The network worker state.
   */
  NetworkWorkerCommState getNetworkWorkerState() const;

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual std::string serialize() override;

private:
  NetworkWorkerCommState networkWorkerState;
};
} // namespace Workers

#endif
