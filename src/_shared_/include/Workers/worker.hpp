#ifndef WORKER_HPP
#define WORKER_HPP

// Project includes
#include <device.hpp>

namespace Workers {

using WorkerId = int;

/**
 * @brief Base class, that represents a worker function.
 */
class Worker {
public:
  /**
   * @brief A worker function, which is called repeatedly.
   */
  virtual void work() = 0;

private:
  WorkerId workerId;
};
} // namespace Workers

#endif