#ifndef SENTRY_WORKER_HPP
#define SENTRY_WORKER_HPP

// Project includes
#include <worker.hpp>

namespace Workers {
class SentryWorker : public Worker {
public:
  SentryWorker();

  virtual ~SentryWorker() override;

  virtual void work(TimePoint timestamp) override;

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
};

} // namespace Workers

#endif