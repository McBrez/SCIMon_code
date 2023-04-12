#ifndef SENTRY_WORKER_HPP
#define SENTRY_WORKER_HPP

// Project includes
#include <is_payload.hpp>
#include <sentry_init_payload.hpp>
#include <worker.hpp>

using namespace Devices;

namespace Workers {

enum ConfigureSubState {
  CONF_SUB_STATE_INVALID,
  CONF_SUB_STATE_INIT,
  CONF_SUB_STATE_CONFIGURE,
  CONF_SUB_STATE_STARTING,
  CONF_SUB_STATE_STARTED
};

class SentryWorker : public Worker {
public:
  SentryWorker();

  virtual ~SentryWorker() override;

  virtual void work(TimePoint timestamp) override;

  virtual bool start();

  virtual bool stop();

  virtual bool initialize(shared_ptr<InitPayload> initPayload) override;

  virtual bool
  configure(shared_ptr<ConfigurationPayload> configPayload) override;

  /**
   * @brief Writes a handshake message to the device.
   * @param writeMsg The handshake message that shall be written to the device.
   * @return True if successful. False otherwise.
   */
  virtual bool write(shared_ptr<HandshakeMessage> writeMsg) override;

  /**
   * @brief Handles a device specific message. Called by
   * write(shared_ptr<WriteDeviceMessage>), if the mssage could not be resolved.
   * @param writeMsg The device specific message.
   * @return True if succesful. False otherwise.
   */
  virtual bool specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) override;

  /**
   * @brief Device-specific read operation, that is executed on each call of
   * read(). Appends to the outgoing message queue.
   * @param timestamp The time at which this method is called.
   * @return A read message.
   */
  virtual list<shared_ptr<DeviceMessage>>
  specificRead(TimePoint timestamp) override;

  /**
   * @brief Handles the response to a write message that has been sent by this
   * object.
   * @param response The response to a write message that has been sent earlier.
   * @return TRUE if the response has been handled successfully. False
   * otherwise.
   */
  virtual bool handleResponse(shared_ptr<ReadDeviceMessage> response) override;

private:
  /// The payload with which this object has been initialized.
  shared_ptr<SentryInitPayload> initPayload;

  /// The current sub state of the configure logic.
  ConfigureSubState configureSubState;

  UserId pumpController;
  DeviceStatus pumpControllerState;

  UserId spectrometer;
  DeviceStatus spectrometerState;

  unique_ptr<thread> workerThread;
  bool runThread;
  bool threadWaiting;

  list<shared_ptr<IsPayload>> isPayloadCache;
  mutex isPayloadCacheMutex;
};
} // namespace Workers

#endif
