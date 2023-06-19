#ifndef WORKER_HPP
#define WORKER_HPP

// Standard includes
#include <ctime>

// Project includes
#include <message_interface.hpp>
#include <status_payload.hpp>

using namespace Messages;
using namespace Utilities;

namespace Workers {

using WorkerId = int;

/**
 * @brief Base class, that represents a worker function.
 */
class Worker : public MessageInterface {
public:
  /**
   * @brief Constructs a new Worker object
   */
  Worker();

  /**
   * @brief Destroys the Worker object
   */
  virtual ~Worker() = 0;

  /**
   * @brief A worker function, which is called repeatedly.
   * @param timestamp The time at which the method has been called.
   */
  virtual void work(TimePoint timestamp) = 0;

  virtual std::list<std::shared_ptr<DeviceMessage>>
  read(TimePoint timestamp) override;

  virtual bool write(std::shared_ptr<WriteDeviceMessage> writeMsg) override;

  virtual bool write(std::shared_ptr<InitDeviceMessage> initMsg) override;

  virtual bool write(std::shared_ptr<ConfigDeviceMessage> configMsg) override;

  /**
   * @brief Tells the worker to start its operation.
   * @return TRUE if worker could be started. False otherwise.
   */
  virtual bool start() = 0;

  /**
   * @brief Tells the worker to stop its operation. After a call to this method,
   * the worker should be put in such a state, that a subsequent call to start()
   * is possible.
   * @return TRUE if worker could be stopped. False otherwise.
   */
  virtual bool stop() = 0;

  virtual bool initialize(std::shared_ptr<InitPayload>) = 0;

  virtual bool configure(std::shared_ptr<ConfigurationPayload>) = 0;

  /**
   * @brief Returns the state of the worker.
   * @return The state of the worker.
   */
  DeviceStatus getState() const;

  /**
   * @brief Constructs the current status of the object.
   * @return Pointer to the current status of the object.
   */
  virtual std::shared_ptr<StatusPayload> constructStatus() override;

  /**
   * @brief Returns the name of the worker.
   * @return A string that identifies the type of the worker.
   */
  virtual std::string getWorkerName() = 0;

protected:
  /// The id of the worker.
  WorkerId workerId;

  /// Used internally for id generation.
  static WorkerId workerIdCounter;

  /// The state of the worker.
  DeviceStatus workerState;
};
} // namespace Workers

#endif
