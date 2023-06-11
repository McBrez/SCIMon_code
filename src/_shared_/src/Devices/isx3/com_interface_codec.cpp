// Project includes
#include <com_interface_codec.hpp>
#include <config_device_message.hpp>
#include <id_payload.hpp>
#include <init_device_message.hpp>
#include <is_payload.hpp>
#include <isx3_ack_payload.hpp>
#include <isx3_is_conf_payload.hpp>

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

std::vector<unsigned char> ComInterfaceCodec::buildCmdSetFeSettings(
    MeasurementConfiguration measConf,
    MeasurmentConfigurationChannel measConfChannel,
    MeasurmentConfigurationRange measConfRange) {

  std::vector<unsigned char> payload;
  payload.push_back(static_cast<unsigned char>(measConf));
  payload.push_back(static_cast<unsigned char>(measConfChannel));
  payload.push_back(static_cast<unsigned char>(measConfRange));

  return this->wrapPayload(payload,
                           Isx3CmdTag::ISX3_COMMAND_TAG_SET_FE_SETTINGS);
}

std::vector<unsigned char> ComInterfaceCodec::buildCmdClearFeSettings() {

  std::vector<unsigned char> payload({0xFF, 0xFF, 0xFF});

  return this->wrapPayload(payload,
                           Isx3CmdTag::ISX3_COMMAND_TAG_SET_FE_SETTINGS);
}

std::vector<unsigned char> ComInterfaceCodec::buildCmdSetExtensionPortChannel(
    int counterPort, int referencePort, int workingSensePort, int workPort) {

  std::vector<unsigned char> payload;
  payload.push_back(static_cast<unsigned char>(counterPort));
  payload.push_back(static_cast<unsigned char>(referencePort));
  payload.push_back(static_cast<unsigned char>(workingSensePort));
  payload.push_back(static_cast<unsigned char>(workPort));

  return this->wrapPayload(
      payload, Isx3CmdTag::ISX3_COMMAND_TAG_SET_EXTENSIONPORT_CHANNEL);
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

std::shared_ptr<ReadPayload>
ComInterfaceCodec::decodeMessage(std::vector<unsigned char> bytes) {

  std::vector<unsigned char> payload;
  Isx3CmdTag commandTag;

  bool unwrapSuccess = this->unwrapPayload(bytes, payload, commandTag);
  if (!unwrapSuccess) {
    return std::shared_ptr<ReadPayload>();
  }

  if (ISX3_COMMAND_TAG_ACK == commandTag) {
    Isx3AckType ackType;
    bool decodeSuccess = this->decodeAck(payload, ackType);
    if (decodeSuccess) {
      return std::shared_ptr<Isx3AckPayload>(
          new Isx3AckPayload(ackType, commandTag));
    } else {
      return std::shared_ptr<ReadPayload>();
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
      return std::shared_ptr<ReadPayload>();
    } else {
      return std::shared_ptr<ReadPayload>(
          new IsPayload(channelNumber, timestamp,
                        std::list<double>({static_cast<double>(fNumber)}),
                        std::list<std::complex<double>>(
                            {static_cast<std::complex<double>>(impedance)})));
    }
  }

  else if (ISX3_COMMAND_GET_DEVICE_ID == commandTag) {
    unsigned char formatVersion;
    short deviceIdentifier;
    short serialNumber;
    int dateOfDeliveryYear;
    int dateOfDeliveryMonth;
    bool decodeSuccess = this->decodeDeviceId(
        payload, formatVersion, deviceIdentifier, serialNumber,
        dateOfDeliveryYear, dateOfDeliveryMonth);
    if (!decodeSuccess) {
      return std::shared_ptr<ReadPayload>();
    } else {
      std::shared_ptr<IdPayload> idPayload(
          new IdPayload("Sciospec", DeviceType::IMPEDANCE_SPECTROMETER,
                        deviceIdentifier, serialNumber));
      return idPayload;
    }
  }

  else {
    // Command not known.
    return std::shared_ptr<ReadPayload>();
  }
}

std::vector<unsigned char>
ComInterfaceCodec::encodeMessage(std::shared_ptr<InitPayload> payload) {
  return std::vector<unsigned char>();
}

std::list<std::vector<unsigned char>> ComInterfaceCodec::encodeMessage(
    std::shared_ptr<ConfigurationPayload> payload) {
  if (!payload) {
    return std::list<std::vector<unsigned char>>();
  }

  // Try to cast it to an isx3 configuration payload.
  std::shared_ptr<Isx3IsConfPayload> confPayload =
      dynamic_pointer_cast<Isx3IsConfPayload>(payload);
  if (!confPayload) {
    // TODO: Try to cast it to a more generic IS configuration and assemble a
    // default setup.
    return std::list<std::vector<unsigned char>>();
  }

  // Do some sanity checks.
  if (confPayload->measurementPoints <= 0) {
    return std::list<std::vector<unsigned char>>();
  }

  FrequencyScale isScale = confPayload->scale == LINEAR_SCALE
                               ? FrequencyScale::FREQ_SCALE_LINEAR
                               : FrequencyScale::FREQ_SCALE_LOGARITHMIC;
  std::list<std::vector<unsigned char>> commandList;
  // Set measurement type to full range impedance spectroscopy.
  commandList.push_back(this->buildCmdInitMeasurement(
      false,
      MeasurementMode::MEASUREMENT_MODE_FULL_RANGE_IMPEDANCE_SPECTROSCOPY));
  // Configure the given frequency std::list.
  commandList.push_back(this->buildCmdSetSetup(
      static_cast<float>(confPayload->frequencyFrom),
      static_cast<float>(confPayload->frequencyTo),
      static_cast<float>(confPayload->measurementPoints), isScale,
      static_cast<float>(confPayload->precision),
      static_cast<float>(confPayload->amplitude)));
  // Clear the front end settings stack.
  commandList.push_back(this->buildCmdClearFeSettings());
  // Set the front end settings.
  commandList.push_back(this->buildCmdSetFeSettings(
      confPayload->measurementConfiguration,
      confPayload->measurementConfChannel, confPayload->measurementConfRange));
  // Set the measurement ports.
  commandList.push_back(this->buildCmdSetExtensionPortChannel(
      confPayload->channel[ChannelFunction::CHAN_FUNC_CP],
      confPayload->channel[ChannelFunction::CHAN_FUNC_RP],
      confPayload->channel[ChannelFunction::CHAN_FUNC_WS],
      confPayload->channel[ChannelFunction::CHAN_FUNC_WP]));

  return commandList;
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
    // Reverse the order of the elements, so that the MSB of the float value
    // is at the lower index.
    std::reverse(buffer.begin(), buffer.end());
  }

  return buffer;
}
bool ComInterfaceCodec::decodeImpedanceData(
    const std::vector<unsigned char> &payload, short &fNumber, float &timestamp,
    short &channelNumber, std::complex<float> &impedance) {

  // Check if payload has expected length.
  // NOTE: Other formats are available. Those are not yet implemented.
  if (payload.size() == 16) {

    fNumber = (payload[0] << 8) + payload[1];

    int timestampIntermediate = (payload[2] << 24) + (payload[3] << 16) +
                                (payload[4] << 8) + payload[5];
    timestamp = *((float *)&timestampIntermediate);

    channelNumber = (payload[6] << 8) + payload[7];

    int realPartIntermediate = (payload[8] << 24) + (payload[9] << 16) +
                               (payload[10] << 8) + payload[11];
    float realPart = *((float *)&realPartIntermediate);

    int imagPartIntermediate = (payload[12] << 24) + (payload[13] << 16) +
                               (payload[14] << 8) + payload[15];
    float imagPart = *((float *)&imagPartIntermediate);

    impedance = std::complex<float>({realPart, imagPart});

    return true;
  }

  else if (payload.size() == 10) {
    fNumber = (payload[0] << 8) + payload[1];

    int realPartIntermediate = (payload[2] << 24) + (payload[3] << 16) +
                               (payload[4] << 8) + payload[5];
    float realPart = *((float *)&realPartIntermediate);

    int imagPartIntermediate = (payload[6] << 24) + (payload[7] << 16) +
                               (payload[8] << 8) + payload[9];
    float imagPart = *((float *)&imagPartIntermediate);

    impedance = std::complex<float>({realPart, imagPart});

    timestamp = -1.0;
    channelNumber = -1;

    return true;
  }

  else {
    return false;
  }
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

bool ComInterfaceCodec::decodeDeviceId(
    const std::vector<unsigned char> &payload, unsigned char &formatVersion,
    short &deviceIdentifier, short &serialNumber, int &dateOfDeliveryYear,
    int &dateOfDeliveryMonth) {

  // Payload of an device id frame has to be at least 7 bytes long.
  if (payload.size() < 7) {
    return false;
  }

  formatVersion = payload[0];
  deviceIdentifier = (payload[1] << 8) + payload[2];
  serialNumber = (payload[3] << 8) + payload[4];
  dateOfDeliveryYear = payload[5] + 2010;
  dateOfDeliveryMonth = payload[6];

  return true;
}

std::vector<unsigned char>
ComInterfaceCodec::buildCmdSetOptions(OptionType optionType, bool enable) {
  std::vector<unsigned char> payload;
  if (OptionType::OPTION_TYPE_ACTIVATE_INVALID == optionType) {
    return std::vector<unsigned char>();
  } else {
    payload.push_back(static_cast<unsigned char>(optionType));
  }
  payload.push_back(enable ? 0x01 : 0x02);

  return this->wrapPayload(payload, Isx3CmdTag::ISX3_COMMAND_TAG_SET_OPTIONS);
}

} // namespace Devices
