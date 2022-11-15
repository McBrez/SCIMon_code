// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <main_control.hpp>

using namespace Devices;

namespace Control {
MainControl::MainControl(list<shared_ptr<Device>> inputDevices,
                         list<shared_ptr<Device>> outputDevices)
    : state(MainControlState::INVALID_CONTROL_STATE),
      inputDevices(inputDevices), outputDevices(outputDevices) {}

bool MainControl::init() {
  this->previousState = MainControlState::INVALID_CONTROL_STATE;
  this->state = MainControlState::INIT;
  return true;
}

bool MainControl::run() {
  // Initial check if the current state is valid.
  if (this->state == MainControlState::INVALID_CONTROL_STATE) {
    // State is invalid. Return here.
    LOG(FATAL) << "Invalid state detected. Exiting control loop.";
    return false;
  }

  // Get the inputs.

  // Determine if a state change shall be done.

  // Apply the current state.
  if (this->state == MainControlState::INIT) {
    // Send init messages to the devices.
    // this->inputDevices.front()->write();
  }

  return false;
}
} // namespace Control
