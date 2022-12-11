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
   * @brief A worker function, which is called repeatedly.
   * @param timestamp The time at which the method has been called.
   */
  virtual void work(TimePoint timestamp) = 0;

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

private:
  WorkerId workerId;
};
} // namespace Workers

#endif