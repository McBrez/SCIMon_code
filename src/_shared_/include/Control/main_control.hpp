#ifndef MAIN_CONTROL_STATE_HPP
#define MAIN_CONTROL_STATE_HPP

// Standard includes
#include <chrono>
#include <list>
#include <memory>
#include <thread>

// Project includes
#include <device.hpp>
#include <message_distributor.hpp>
#include <utilities.hpp>
#include <worker.hpp>

using namespace Utilities;

using namespace chrono_literals;
using namespace Devices;
using namespace Workers;

namespace Control {

/**
 * @brief Identifies the states of the main control loop.
 */
enum MainControlState {
  /// Invalid main control state.
  MAIN_CONTROL_INVALID,
  /// Main control is initializing.
  MAIN_CONTROL_INIT,
  /// Main control has been started and is running.
  MAIN_CONTROL_STARTED,
  /// Main control has been stopped.
  MAIN_CONTROL_STOPPED
};

/**
 * @brief Encapsulates a control loop, that tries to achive soft realtime
 * capabilities.
 */
class MainControl {
private:
  /// A std::list of references to devices.
  std::list<std::shared_ptr<Device>> devices;

  /// A std::list of references to workers.
  std::list<std::shared_ptr<Worker>> workers;

  /// Reference to the thread, that executes the worker function.
  unique_ptr<thread> workerThread;

  /// The state of the main control loop.
  MainControlState mainControlState;

  /// Reference to the message distributor.
  unique_ptr<MessageDistributor> messageDistributor;

  /// The loop interval, the control loop tries to achieve. In milliseconds.
  Duration targetInterval;

  /// The timestamp of the last call to work().
  TimePoint lastTimestamp;

public:
  MainControl(Duration targetInterval);

  /**
   * @brief Starts the main control loop in another thread.
   * @return TRUE if the control loop has been started. False otherwise.
   */
  bool start();

  /**
   * @brief Stops the main control loop. A subsequent call to start() can start
   * the control loop again.
   * @return TRUE if the control loop has been stopped. False otherwise.
   */
  bool stop();

  /**
   * @brief Implements the worker loop. Runs in a seperate thread.
   */
  void work();

  bool addDevice();
  bool removeDevice();

  bool addAction();
  bool removeAction();

  MainControlState getState();
};
} // namespace Control

#endif