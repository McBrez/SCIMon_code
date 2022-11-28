// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <config_is_message.hpp>
#include <device_isx3.hpp>
#include <utility.hpp>

namespace Devices {

const std::vector<unsigned char> DeviceIsx3::knownCommandTags = {
    ISX3_COMMAND_TAG_ACK, ISX3_COMMAND_TAG_SET_SETUP};

DeviceIsx3::DeviceIsx3() : Device() {}

DeviceIsx3::~DeviceIsx3(){};

bool DeviceIsx3::configure(DeviceConfiguration *deviceConfiguration) {
  return true;
}

bool DeviceIsx3::open() { return true; }

bool DeviceIsx3::close() { return true; }

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
  // Try to downcast the generic config message to impedance spectrometer
  // config message.
  shared_ptr<ConfigIsMessage> msg =
      dynamic_pointer_cast<ConfigIsMessage>(configMsg);
  if (!msg) {
    LOG(ERROR) << "Received an invalid message. It will be ingored.";
    return false;
  }

  // Encode the contents of configMsg into a command for ISX3.
  std::vector<unsigned char> buffer;

  // [CT] - command tag
  buffer.push_back(ISX3_COMMAND_TAG_SET_SETUP);
  // [LE] - Length of [CD]
  buffer.push_back(0x16);
  // [CO] - command option (frequency range)
  buffer.push_back(0x03);
  // [CD] - command data
  // NOTE: It is assumed here, that float always consists of 4 byte. This
  // obviously will not work on machines that do not follow such conventions.
  // -- Starting frequency
  float startFreq = static_cast<float>(msg->frequencyFrom);
  char startFreqRawBytes[sizeof(float)];
  memcpy(startFreqRawBytes, &startFreq, sizeof(startFreq));
  buffer.push_back(startFreqRawBytes[3]);
  buffer.push_back(startFreqRawBytes[2]);
  buffer.push_back(startFreqRawBytes[1]);
  buffer.push_back(startFreqRawBytes[0]);
  // -- Ending frequency
  float stopFreq = static_cast<float>(msg->frequencyTo);
  char stopFreqRawBytes[sizeof(float)];
  memcpy(stopFreqRawBytes, &stopFreq, sizeof(stopFreq));
  buffer.push_back(stopFreqRawBytes[3]);
  buffer.push_back(stopFreqRawBytes[2]);
  buffer.push_back(stopFreqRawBytes[1]);
  buffer.push_back(stopFreqRawBytes[0]);
  // -- count measurement points
  float measurementPoints = static_cast<float>(msg->measurementPoints);
  char measurementPointsRawBytes[sizeof(float)];
  memcpy(measurementPointsRawBytes, &measurementPoints,
         sizeof(measurementPoints));
  buffer.push_back(measurementPointsRawBytes[3]);
  buffer.push_back(measurementPointsRawBytes[2]);
  buffer.push_back(measurementPointsRawBytes[1]);
  buffer.push_back(measurementPointsRawBytes[0]);
  // -- scale
  unsigned char scale;
  if (msg->scale == IsScale::LINEAR) {
    scale = 0x00;
  } else if (msg->scale == IsScale::LOGARITHMIC) {
    scale = 0x01;
  }
  buffer.push_back(scale);
  // -- precision
  // NOTE: Hardcoded to 1.0 at the moment.
  buffer.push_back(0x3F);
  buffer.push_back(0x80);
  buffer.push_back(0x00);
  buffer.push_back(0x00);
  // -- amplitude
  float amplitude = static_cast<float>(msg->amplitude);
  char amplitudeRawBytes[sizeof(float)];
  memcpy(amplitudeRawBytes, &amplitude, sizeof(amplitude));
  buffer.push_back(amplitudeRawBytes[3]);
  buffer.push_back(amplitudeRawBytes[2]);
  buffer.push_back(amplitudeRawBytes[1]);
  buffer.push_back(amplitudeRawBytes[0]);

  // [CT] - command tag
  buffer.push_back(ISX3_COMMAND_TAG_SET_SETUP);

  int retVal = this->writeToIsx3(buffer);
  return retVal > 0;
}

bool DeviceIsx3::write(shared_ptr<WriteDeviceMessage> writeMsg) { return true; }

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
  auto cmdTermination = std::find(readBuffer.begin(), readBuffer.end(), "\n");
  if (cmdTermination == readBuffer.end()) {
    // No command found. Return here.
    return shared_ptr<ReadDeviceMessage>();
  }

  // Parse buffer into string.
  string command = "";

  if (command == "RESET") {
  }

  return shared_ptr<ReadDeviceMessage>();
}

string DeviceIsx3::getDeviceTypeName() { return Isx3DeviceTypeName; }

} // namespace Devices
