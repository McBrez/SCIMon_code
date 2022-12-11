// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <main_control.hpp>

using namespace Devices;

namespace Control {

MainControl::MainControl(Duration targetInterval)
    : messageDistributor(new MessageDistributor()),
      mainControlState(MAIN_CONTROL_INVALID), targetInterval(targetInterval) {}

bool MainControl::start() {
  // MainControl can only be started, if it has freshly been created, or if it
  // has been stopped.
  if (this->mainControlState == MainControlState::MAIN_CONTROL_INVALID ||
      this->mainControlState == MainControlState::MAIN_CONTROL_STOPPED) {

    this->mainControlState = MainControlState::MAIN_CONTROL_INIT;
    this->workerThread.reset(new thread(&MainControl::work, this));
    return true;
  } else {
    LOG(WARNING) << "Can not start MainControl, as it is already started.";
    return false;
  }
}

bool MainControl::stop() {
  this->mainControlState = MainControlState::MAIN_CONTROL_STOPPED;
  this->workerThread->join();
  this->workerThread.reset();
  return true;
}

void MainControl::work() {
  while (true) {
    if (MainControlState::MAIN_CONTROL_INIT == this->mainControlState) {
      chrono::time_point currentTimestamp = chrono::system_clock::now();
      this->mainControlState = MainControlState::MAIN_CONTROL_STARTED;
      LOG(INFO) << "Main control has been initialized";
    } else if (MainControlState::MAIN_CONTROL_STARTED ==
               this->mainControlState) {
      // Get the time of this call.
      chrono::time_point currentTimestamp = chrono::system_clock::now();

      // Read from the devices.
      for (auto device : this->devices) {
        this->messageDistributor->takeMessage(device,
                                              device->read(currentTimestamp));
      }

      // Distribute readings from the devices.
      this->messageDistributor->deliverMessages();

      // Run the workers.
      for (auto worker : this->workers) {
        worker->work(currentTimestamp);
        this->messageDistributor->takeMessage(worker,
                                              worker->read(currentTimestamp));
      }

      // Distribute results from the workers.
      this->messageDistributor->deliverMessages();

      // Check timeliness.
      if (chrono::system_clock::now() - currentTimestamp >
          this->targetInterval) {
        LOG(WARNING) << "Can not keep up!";
        // TODO: Implement measured, that helps the loop to keep up.
      }

      auto oldCurrentTimestamp = currentTimestamp;
      this->lastTimestamp = currentTimestamp;
      this_thread::sleep_until(oldCurrentTimestamp + this->targetInterval);
    }

    else if (MainControlState::MAIN_CONTROL_STOPPED == this->mainControlState) {
      LOG(INFO) << "Main control is stopping...";
      // Stop all devices and workers.
      for (auto device : this->devices) {
        device->stop();
      }
      for (auto worker : this->workers) {
        worker->stop();
      }

      // Return from loop.
      LOG(INFO) << "Main control has stopped";
      return;
    }

    else {
      // Invalid state. Return ungracefully.
      LOG(ERROR) << "Detected invalid main control state. Aborting.";
      return;
    }
  }
}

bool MainControl::addDevice() { return false; }
bool MainControl::removeDevice() { return false; }

bool MainControl::addAction() { return false; }
bool MainControl::removeAction() { return false; }

MainControlState MainControl::getState() { return this->mainControlState; }

} // namespace Control
