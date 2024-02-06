#ifndef WORKER_HPP
#define WORKER_HPP

// Standard includes
#include <ctime>

// Project includes
#include <data_manager.hpp>
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
  Worker(unsigned int dataManagerMeasurementLevel,
         Utilities::DataManagerType dataManagerType =
             DataManagerType::DATAMANAGER_TYPE_HDF);

  /**
   * @brief Destroys the Worker object
   */
  virtual ~Worker() = 0;

  /**
   * @brief A worker function, which is called repeatedly.
   * @param timestamp The time at which the method has been called.
   */
  virtual void work(TimePoint timestamp) = 0;

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
};
} // namespace Workers

#endif
