#ifndef CONTROL_WORKER_HPP
#define CONTROL_WORKER_HPP

// Project includes
#include <worker.hpp>

using namespace Devices;

namespace Workers {

/**
 * @brief Identfies the sub states of the control worker.
 */
enum ControlWorkerSubState {
  /// Invalid sub state.
  CONTROL_WORKER_SUBSTATE_IVALID = 0x00,
  /// It is waited for the connect command
  CONTROL_WORKER_SUBSTATE_WAITING_FOR_CONNECTION = 0x01,
  /// Control worker is connecting
  CONTROL_WORKER_SUBSTATE_CONNECTING = 0x02,
  /// Control worker is connected.
  CONTROL_WORKER_SUBSTATE_CONNECTED = 0x03
};

/**
 * @brief Worker that interfaces with the UI part of SCIMon.
 */
class ControlWorker : public Worker {
public:
  ControlWorker();

  virtual ~ControlWorker() override;

  virtual void work(TimePoint timestamp) override;

  virtual bool start() override;

  virtual bool stop() override;

  virtual bool initialize(std::shared_ptr<InitPayload> initPayload) override;

  virtual bool
  configure(std::shared_ptr<ConfigurationPayload> configPayload) override;

  /**
   * @brief Writes a handshake message to the device.
   * @param writeMsg The handshake message that shall be written to the device.
   * @return True if successful. False otherwise.
   */
  virtual bool write(std::shared_ptr<HandshakeMessage> writeMsg) override;

  /**
   * @brief Handles a device specific message. Called by
   * write std::shared_ptr<WriteDeviceMessage>), if the mssage could not be
   * resolved.
   * @param writeMsg The device specific message.
   * @return True if succesful. False otherwise.
   */
  virtual bool
  specificWrite(std::shared_ptr<WriteDeviceMessage> writeMsg) override;

  /**
   * @brief Device-specific read operation, that is executed on each call of
   * read(). Appends to the outgoing message queue.
   * @param timestamp The time at which this method is called.
   * @return A read message.
   */
  virtual std::list<std::shared_ptr<DeviceMessage>>
  specificRead(TimePoint timestamp) override;

  /**
   * @brief Handles the response to a write message that has been sent by this
   * object.
   * @param response The response to a write message that has been sent earlier.
   * @return TRUE if the response has been handled successfully. False
   * otherwise.
   */
  virtual bool
  handleResponse(std::shared_ptr<ReadDeviceMessage> response) override;

  /**
   * @brief Returns the name of the worker.
   * @return A string that identifies the type of the worker.
   */
  virtual std::string getWorkerName() override;

  /**
   * @brief Try to connect to the specified host.
   * @param ip The IP to which a connection shall be established.
   * @param port The port to which a connection shall be established.
   * @return TRUE if connection was successfull. FALSE otherwise.
   */
  bool startConnect(std::string ip, int port);

  /**
   * @brief Starts the configuration of the control worker.
   * I.e. The control worker queries all remote message
   * interface object.
   * @return
   */
  bool startConfig();

  ControlWorkerSubState getControlWorkerState() const;

private:
  /// The id of the network worker.
  UserId networkWorkerId;

  /// The currently active sub state.
  ControlWorkerSubState controlWorkerSubState;
};
} // namespace Workers

#endif
