#ifndef MAIN_CONTROL_STATE_HPP
#define MAIN_CONTROL_STATE_HPP

// Standard includes
#include <list>
#include <memory>
#include <thread>

// Project includes
#include <device.hpp>

using namespace std;
using namespace Devices;

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
 * @brief Encapsulates a control loop, that is executed in a separate thread.
 */
class MainControl {
private:
  /// A list of references to devices, which are
  list<shared_ptr<Device>> devices;

  /// Reference to the thread, that executes the worker function.
  unique_ptr<thread> workerThread;

  /// The state of the main control loop.
  MainControlState mainControlState;

public:
  MainControl();

  bool start();

  bool stop();

  void work();

  bool addDevice();
  bool removeDevice();

  bool addAction();
  bool removeAction();
};
} // namespace Control

#endif