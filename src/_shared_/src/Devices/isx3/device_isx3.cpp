// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <device_isx3.hpp>
#include <device_status_message.hpp>
#include <is_configuration.hpp>
#include <utilities.hpp>

namespace Devices {

const std::vector<unsigned char> DeviceIsx3::knownCommandTags = {
    ISX3_COMMAND_TAG_ACK, ISX3_COMMAND_TAG_SET_SETUP};

DeviceIsx3::DeviceIsx3() : Device() {}

DeviceIsx3::~DeviceIsx3(){};

bool DeviceIsx3::write(shared_ptr<InitDeviceMessage> initMsg) {
  // Try to cast the init message.
  shared_ptr<InitMessageIsx3> castedMsg =
      dynamic_pointer_cast<InitMessageIsx3>(initMsg);
  if (!castedMsg) {
    // Casting was not successful. Return here with an error code.
    return false;
  }

  // Start the OS specific initialization.
  int retVal = this->initIsx3(castedMsg);
  return retVal == 0 ? true : false;
}

bool DeviceIsx3::write(shared_ptr<ConfigDeviceMessage> configMsg) {
  // Configure ISX3 according to the given data.
  return this->configure(configMsg->getConfiguration());
}

bool DeviceIsx3::write(shared_ptr<WriteDeviceMessage> writeMsg) {
  if (WriteDeviceTopic::RUN_TOPIC == writeMsg->getTopic()) {
    // Start the device.

  } else {
    // Unusupported topic.
    return false;
  }
}

shared_ptr<ReadDeviceMessage> DeviceIsx3::read() {
  int bytesRead = this->readFromIsx3();
  LOG(DEBUG) << "Read " << bytesRead << " bytes from ISX3.";

  std::string bufferContent;
  for (char ch : this->readBuffer) {
    bufferContent += ch;
  }

  LOG(DEBUG) << "Buffer content: " << bufferContent;

  // Try to extract a frame from the read buffer.
  return this->interpretBuffer(this->readBuffer);
}

shared_ptr<ReadDeviceMessage>
DeviceIsx3::interpretBuffer(std::vector<unsigned char> &readBuffer) {
  // Find new line character, which terminates commands.
  LOG(DEBUG) << "Interpreting the buffer content: "
             << string(this->readBuffer.begin(), this->readBuffer.end());
  auto cmdTermination = std::find(readBuffer.begin(), readBuffer.end(), '\n');
  if (cmdTermination == readBuffer.end()) {
    // No command found. Return here.
    LOG(DEBUG) << "No command terminator found. Returning empty message";
    return shared_ptr<ReadDeviceMessage>();
  }
  // Parse buffer into string.
  string command(readBuffer.begin(), cmdTermination - 1);
  // Remove the extracted string from the buffer.
  // TODO: Erasing from the front of a vector is quite inefficient. Replacing
  // the vector with a list should be more efficient.
  this->readBuffer.erase(readBuffer.begin(), ++cmdTermination);

  // Split the extracted string.
  unsigned char commandSplitToken = ' ';

  vector<string> splittedCmd = Utilities::split(command, commandSplitToken);
  if (splittedCmd.size() == 0) {
    // No command found. Return here.
    LOG(DEBUG) << "No command found. Returning empty message";
    return shared_ptr<ReadDeviceMessage>();
  }

  // Check the first field.
  if ("ack" == splittedCmd[0]) {
    // This is an acknowledge. Return it.
    LOG(DEBUG) << "Found acknowledge";
    return shared_ptr<ReadDeviceMessage>(new ReadDeviceMessage("ack"));
  }

  else if ("deviceStatus" == splittedCmd[0]) {
    // This is a response to the getDeviceStatus command. The second field
    // contains the device status. Check if the decoded string vector is long
    // enough.
    if (splittedCmd.size() < 2) {
      // It is not. Return here.
      return shared_ptr<ReadDeviceMessage>();
    }

    // Check the status.
    if ("busy" == splittedCmd[1]) {
      return shared_ptr<ReadDeviceMessage>(
          new DeviceStatusMessage(DeviceStatus::BUSY));
    } else if ("idle" == splittedCmd[1]) {
      return shared_ptr<ReadDeviceMessage>(
          new DeviceStatusMessage(DeviceStatus::IDLE));
    } else {
      return shared_ptr<ReadDeviceMessage>(
          new DeviceStatusMessage(DeviceStatus::UNKNOWN_DEVICE_STATUS));
    }
  }

  else if ("error" == splittedCmd[0]) {
    // Error has been returned. They are not handled yet, so return an empty
    // message.
    LOG(DEBUG) << "Device returner an error: \"" << command << "\".";
    return shared_ptr<ReadDeviceMessage>();
  }

  else {
    // Could not extract a valid message. Return the analyzed part of the buffer
    // as message.
    LOG(DEBUG) << "Found unsupported message: " << command;
    return shared_ptr<ReadDeviceMessage>(new ReadDeviceMessage(command));
  }
}

string DeviceIsx3::getDeviceTypeName() { return Isx3DeviceTypeName; }

} // namespace Devices
