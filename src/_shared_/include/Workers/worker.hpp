#ifndef WORKER_HPP
#define WORKER_HPP

// Standard includes
#include <ctime>

// Project includes
#include <message_interface.hpp>

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

  virtual list<shared_ptr<DeviceMessage>> read(TimePoint timestamp) override;

  bool write(shared_ptr<WriteDeviceMessage> writeMsg) override;

  bool write(shared_ptr<InitDeviceMessage> initMsg) override;

  bool write(shared_ptr<ConfigDeviceMessage> configMsg) override;

  /**
   * @brief Tells the worker to start its operation.
   * @return TRUE if worker could be started. False otherwise.
   */
  bool start();

  /**
   * @brief Tells the worker to stop its operation. After a call to this method,
   * the worker should be put in such a state, that a subsequent call to start()
   * is possible.
   * @return TRUE if worker could be stopped. False otherwise.
   */
  bool stop();

private:
  WorkerId workerId;
};
} // namespace Workers

#endif