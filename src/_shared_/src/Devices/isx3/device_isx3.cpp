// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <ack_message_isx3.hpp>
#include <config_is_message.hpp>
#include <device_isx3.hpp>

namespace Devices {

const std::vector<unsigned char> DeviceIsx3::knownCommandTags = {
    ISX3_COMMAND_TAG_ACK, ISX3_COMMAND_TAG_SET_SETUP};

DeviceIsx3::DeviceIsx3() : Device() {}

bool DeviceIsx3::configure(DeviceConfiguration *deviceConfiguration) {
  return true;
}

bool DeviceIsx3::open() { return true; }

bool DeviceIsx3::close() { return true; }

bool DeviceIsx3::write(shared_ptr<InitDeviceMessage> initMsg) {

  std::vector<unsigned char> initCmd1 = {0x15, 0x03, 0xDF, 0xEF, 0xFF, 0x15};
  std::vector<unsigned char> initCmd2 = {0x0C, 0x01, 0x08, 0x0C};
  std::vector<unsigned char> initCmd3 = {0x17, 0x00, 0x17};
  int retVal1 = this->writeToIsx3(initCmd1, true);
  int retVal2 = this->writeToIsx3(initCmd2, true);
  int retVal3 = this->writeToIsx3(initCmd3, true);

  return true;
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

  // Try to extract a frame from the read buffer.
  return this->interpretBuffer(this->readBuffer);
}

shared_ptr<ReadDeviceMessage>
DeviceIsx3::interpretBuffer(std::vector<unsigned char> &readBuffer) {

  // Iterate over known command tags and look for them in the read buffer.
  for (auto commandTag : this->knownCommandTags) {
    auto frameBegin =
        std::find(this->readBuffer.begin(), this->readBuffer.end(), commandTag);

    if (frameBegin == this->readBuffer.end()) {
      // command tag has not been found. Continue with the next one.
      continue;
    }

    // In a valid command frame, the length is indicated by the byte after the
    // command tag. Evaluate that now.
    unsigned char dataLength = *(frameBegin + 1);
    // In avalid command frame, the command is terminated by the command tag.
    // Check if that is the case.
    auto frameEnd = frameBegin + dataLength;
    if (frameEnd != this->readBuffer.end() && *frameEnd == commandTag) {
      // This seems to be a valid command frame. Decode it now.
      shared_ptr<ReadDeviceMessage> decodedMessage;

      // ACKNOWLEDGMENT
      if (Isx3CmdType::ISX3_COMMAND_TAG_ACK == *frameBegin) {
        if (*(frameBegin + 1) != 0x01) {
          LOG(WARNING)
              << "Malformed Acknowledge frame received. This will be ignored";
          return shared_ptr<ReadDeviceMessage>();
        }
        Isx3AckType ackType = static_cast<Isx3AckType>(*(frameBegin + 2));

        decodedMessage.reset(new AckMessageIsx3(ackType));
      } else {
        // A command frame has been detected, but its decoding is not yet
        // implemented.
        LOG(DEBUG) << "A command frame has been detected, but its decoding is "
                      "not yet implemented.";
      }

      // Remove the bytes from the beginning of the bytes to the end of the
      // frame. This may delete bytes prior to the detected frame. As most
      // likely anyhow wont be decoded this should be safe.
      this->readBuffer.erase(this->readBuffer.begin(), frameEnd + 1);

      return decodedMessage;
    }
  }

  LOG(DEBUG) << "Could not extract any frames from the read buffer of length: "
             << this->readBuffer.size();
  return shared_ptr<ReadDeviceMessage>();
}

} // namespace Devices
