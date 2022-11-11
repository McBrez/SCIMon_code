#ifndef MAIN_CONTROL_STATE_HPP
#define MAIN_CONTROL_STATE_HPP

// Standard includes
#include <list>
#include <memory>

// Project includes
#include <device.hpp>

using namespace std;
using namespace Devices;

namespace Control {
enum MainControlState {
  INVALID_CONTROL_STATE = -1,
  INIT,
  CALIBRATION,
  IDLE,
  RUNNING
};

/**
 * @brief Encapsulates a state machine that implements a control logic.
 */
class MainControl {
private:
  /// The previously active main control state.
  MainControlState previousState;

  /// The currently active main control state.
  MainControlState state;

  /// A list of references to input devices.
  list<shared_ptr<Device>> inputDevices;

  /// A list of references to output devices.
  list<shared_ptr<Device>> outputDevices;

public:
  MainControl(list<shared_ptr<Device>> inputDevices,
              list<shared_ptr<Device>> outputDevices);

  /**
   * The main control function. Has to be called in a loop.
   * @return False if the main control had do abort due to some error. True
   * otherwise.
   */
  bool run();

  /**
   * Initializes the main control.
   * @return True if initialization was successfull. False otherwise.
   */
  bool init();
};
} // namespace Control

#endif