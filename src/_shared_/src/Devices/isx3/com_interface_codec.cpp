// Project includes
#include <com_interface_codec.hpp>
#include <is_payload.hpp>
#include <isx3_ack_payload.hpp>

namespace Devices {

ComInterfaceCodec::ComInterfaceCodec() {}

std::vector<unsigned char> ComInterfaceCodec::buildCmdResetSystem() {
  // Payload is empty.
  std::vector<unsigned char> payload;

  return this->wrapPayload(payload, Isx3CmdTag::ISX3_COMMAND_TAG_RESET_SYSTEM);
}

std::vector<unsigned char> ComInterfaceCodec::buildCmdGetDeviceId() {
  // Payload is empty.
  std::vector<unsigned char> payload;

  return this->wrapPayload(payload, Isx3CmdTag::ISX3_COMMAND_GET_DEVICE_ID);
}

std::vector<unsigned char>
ComInterfaceCodec::buildCmdInitMeasurement(bool readWrite,
                                           MeasurementMode measurementMode) {

  std::vector<unsigned char> payload;

  if (readWrite) {
    payload.push_back(0x80);
  } else {
    payload.push_back(0x00);
    payload.push_back(static_cast<unsigned char>(measurementMode));
  }

  return this->wrapPayload(payload,
                           Isx3CmdTag::ISX3_COMMAND_TAG_INIT_MEASUREMENT);
}

std::vector<unsigned char> ComInterfaceCodec::buildCmdSetSetup() {

  std::vector<unsigned char> payload({static_cast<unsigned char>(
      SetSetupCommandOption::SET_SETUP_CMD_OPTION_INIT_MEASUREMENT)});
  return this->wrapPayload(payload, Isx3CmdTag::ISX3_COMMAND_TAG_SET_SETUP);
}

std::vector<unsigned char>
ComInterfaceCodec::buildCmdSetSetup(float frequencyPoint, float precision,
                                    float amplitude) {
  std::vector<unsigned char> payload;
  payload.reserve(13);
  payload.push_back(static_cast<unsigned char>(
      SetSetupCommandOption::SET_SETUP_CMD_OPTION_ADD_FREQ_POINT));

  std::vector<unsigned char> frequencyPointRaw =
      this->getRawBytes(frequencyPoint);
  payload.insert(payload.end(), frequencyPointRaw.begin(),
                 frequencyPointRaw.end());

  std::vector<unsigned char> precisionRaw = this->getRawBytes(precision);
  payload.insert(payload.end(), precisionRaw.begin(), precisionRaw.end());

  std::vector<unsigned char> amplitudeRaw = this->getRawBytes(amplitude);
  payload.insert(payload.end(), amplitudeRaw.begin(), amplitudeRaw.end());

  return this->wrapPayload(payload, Isx3CmdTag::ISX3_COMMAND_TAG_SET_SETUP);
}

std::vector<unsigned char>
ComInterfaceCodec::buildCmdSetSetup(float fStart, float fStop, float fCount,
                                    FrequencyScale frequencyScale,
                                    float precision, float amplitude) {
  std::vector<unsigned char> payload;
  payload.reserve(22);
  payload.push_back(static_cast<unsigned char>(
      SetSetupCommandOption::SET_SETUP_CMD_OPTION_ADD_FREQ_LIST));

  std::vector<unsigned char> fStartRaw = this->getRawBytes(fStart);
  payload.insert(payload.end(), fStartRaw.begin(), fStartRaw.end());

  std::vector<unsigned char> fStopRaw = this->getRawBytes(fStop);
  payload.insert(payload.end(), fStopRaw.begin(), fStopRaw.end());

  std::vector<unsigned char> fCountRaw = this->getRawBytes(fCount);
  payload.insert(payload.end(), fCountRaw.begin(), fCountRaw.end());

  payload.push_back(static_cast<unsigned int>(frequencyScale));

  std::vector<unsigned char> precisionRaw = this->getRawBytes(precision);
  payload.insert(payload.end(), precisionRaw.begin(), precisionRaw.end());

  std::vector<unsigned char> amplitudeRaw = this->getRawBytes(amplitude);
  payload.insert(payload.end(), amplitudeRaw.begin(), amplitudeRaw.end());

  return this->wrapPayload(payload, Isx3CmdTag::ISX3_COMMAND_TAG_SET_SETUP);
}

shared_ptr<ReadPayload>
ComInterfaceCodec::decodeMessage(std::vector<unsigned char> bytes) {

  std::vector<unsigned char> payload;
  Isx3CmdTag commandTag;

  bool unwrapSuccess = this->unwrapPayload(bytes, payload, commandTag);
  if (!unwrapSuccess) {
    return shared_ptr<ReadPayload>();
  }

  if (ISX3_COMMAND_TAG_ACK == commandTag) {
    Isx3AckType ackType;
    bool decodeSuccess = this->decodeAck(payload, ackType);
    if (decodeSuccess) {
      return shared_ptr<Isx3AckPayload>(
          new Isx3AckPayload(ackType, commandTag));
    } else {
      return shared_ptr<ReadPayload>();
    }
  }

  else if (ISX3_COMMAND_TAG_START_IMPEDANCE_MEAS == commandTag) {
    short fNumber;
    float timestamp;
    short channelNumber;
    std::complex<float> impedance;
    bool decodeSuccess = this->decodeImpedanceData(payload, fNumber, timestamp,
                                                   channelNumber, impedance);
    if (!decodeSuccess) {
      return shared_ptr<ReadPayload>();
    } else {
      return shared_ptr<ReadPayload>(
          new IsPayload(channelNumber, timestamp,
                        std::list<double>({static_cast<double>(fNumber)}),
                        std::list<complex<double>>({impedance})));
    }
  }

  else {
    // Command not known.
    return shared_ptr<ReadPayload>();
  }
}

std::vector<unsigned char>
ComInterfaceCodec::encodeMessage(shared_ptr<DeviceMessage> message) {
  return std::vector<unsigned char>();
}

std::vector<unsigned char>
ComInterfaceCodec::wrapPayload(const std::vector<unsigned char> &payload,
                               Isx3CmdTag cmdTag) {

  std::vector<unsigned char> buffer;
  buffer.reserve(payload.size() + 3);
  buffer.push_back(static_cast<unsigned char>(cmdTag));
  buffer.push_back(static_cast<unsigned char>(payload.size()));
  for (auto payloadByte : payload) {
    buffer.push_back(payloadByte);
  }
  buffer.push_back(static_cast<unsigned char>(cmdTag));

  return buffer;
}

bool ComInterfaceCodec::unwrapPayload(const std::vector<unsigned char> &frame,
                                      std::vector<unsigned char> &payload,
                                      Isx3CmdTag &commandTag) {
  // Do some consistency checks.
  // The minimum size of a COM interface frame is 3. Check that here.
  if (frame.size() < 3) {
    commandTag = Isx3CmdTag::ISX3_COMMAND_TAG_INVALID;
    return false;
  }
  // Frame has to start and end with the command tag.
  if (frame.front() != frame.back()) {
    commandTag = Isx3CmdTag::ISX3_COMMAND_TAG_INVALID;
    return false;
  }
  // The second byte encodes the length of the payload.
  if (frame[1] != (frame.size() - 3)) {
    commandTag = Isx3CmdTag::ISX3_COMMAND_TAG_INVALID;
    return false;
  }
  // FIXME: A check if commandTag is valid is missing.

  // Frame seems good. Start unwraping.
  payload = std::vector<unsigned char>(frame.begin() + 2, frame.end() - 1);
  commandTag = static_cast<Isx3CmdTag>(frame.front());
  return true;
}

std::vector<unsigned char>
ComInterfaceCodec::buildCmdStartImpedanceMeasurement(bool state) {

  std::vector<unsigned char> payload;

  if (state) {
    payload.push_back(0x01);
  } else {
    payload.push_back(0x00);
  }

  return this->wrapPayload(payload,
                           Isx3CmdTag::ISX3_COMMAND_TAG_START_IMPEDANCE_MEAS);
}

std::vector<unsigned char> ComInterfaceCodec::buildCmdStartImpedanceMeasurement(
    unsigned short burstCount) {

  std::vector<unsigned char> payload;
  payload.reserve(3);
  payload.push_back(0x02);

  std::vector<unsigned char> burstCountRaw = this->getRawBytes(burstCount);
  payload.insert(payload.end(), burstCountRaw.begin(), burstCountRaw.end());

  return this->wrapPayload(payload,
                           Isx3CmdTag::ISX3_COMMAND_TAG_START_IMPEDANCE_MEAS);
}

template <typename T>
std::vector<unsigned char> ComInterfaceCodec::getRawBytes(T value,
                                                          bool bigEndian) {
  unsigned char *ptr = (unsigned char *)&value;
  std::vector<unsigned char> buffer;
  buffer.reserve(sizeof(T));
  for (int i = 0; i < sizeof(T); i++) {
    buffer.push_back(*(ptr + i));
  }

  if (bigEndian) {
    // Reverse the order of the elements, so that the MSB of the float value is
    // at the lower index.
    std::reverse(buffer.begin(), buffer.end());
  }

  return buffer;
}
bool ComInterfaceCodec::decodeImpedanceData(
    const std::vector<unsigned char> &payload, short &fNumber, float &timestamp,
    short &channelNumber, std::complex<float> &impedance) {

  // payload has to be 16 bytes long.
  if (payload.size() != 16) {
    return false;
  }

  fNumber = *((short *)&payload[0]);
  timestamp = *((float *)&payload[5]);
  channelNumber = *((short *)&payload[7]);
  float realPart = *((float *)&payload[11]);
  float imagPart = *((float *)&payload[15]);
  impedance = std::complex<float>({realPart, imagPart});

  return true;
}

bool ComInterfaceCodec::decodeAck(const std::vector<unsigned char> &payload,
                                  Isx3AckType &ackType) {

  // Payload has to be exactly 1 item long.
  if (payload.size() != 1) {
    return false;
  }

  ackType = static_cast<Isx3AckType>(payload[0]);
  return true;
}

} // namespace Devices
