// Standard includes
#include <chrono>
#include <random>

// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <common.hpp>
#include <device_isx3.hpp>
#include <is_configuration.hpp>
#include <isx3_ack_payload.hpp>
#include <utilities.hpp>

namespace Devices {

std::unique_ptr<std::thread> writeThread;
bool doWriteThread = false;
std::vector<double> frequencies;
void writeThreadWorker(DataManager *dataManager, DeviceIsx3 *device) {
  static double counter = 0.0;
  while (doWriteThread) {

    std::vector<Impedance> impedances;
    for (auto frequency : frequencies) {
      impedances.push_back(Impedance(++counter, ++counter));
    }
    ImpedanceSpectrum impedanceSpectrum;
    Utilities::joinImpedanceSpectrum(frequencies, impedances,
                                     impedanceSpectrum);
    dataManager->write(Core::getNow(), device->getCurrentSpectrumKey(),
                       Value(impedanceSpectrum));

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

DeviceIsx3::DeviceIsx3()
    : Device(DeviceType::IMPEDANCE_SPECTROMETER),
      socketWrapper(SocketWrapper::getSocketWrapper()),
      isx3CommThreadState(ISX3_COMM_THREAD_STATE_INVALID), doComm(false) {}

DeviceIsx3::~DeviceIsx3() {
  this->doComm = false;
  this->commThread->join();
  this->socketWrapper->close();

  if (doWriteThread) {
    doWriteThread = false;
    writeThread->join();
  }
};

std::string DeviceIsx3::getDeviceTypeName() { return ISX3_DEVICE_TYPE_NAME; }

std::shared_ptr<Isx3CmdAckStruct>
DeviceIsx3::pushToSendBuffer(const std::vector<unsigned char> &bytes) {

  // Frames smaller than three bytes do not make sense.
  if (bytes.size() < 3) {
    LOG(WARNING) << "A frame smaller than 3 bytes would have been sent. Frames "
                    "can not be smaller than 3.";
    return std::shared_ptr<Isx3CmdAckStruct>();
  }

  // Construct the ack structure.
  std::shared_ptr<Isx3CmdAckStruct> ackStruct(new Isx3CmdAckStruct);
  ackStruct->acked = Isx3AckType::ISX3_ACK_TYPE_INVALID;
  ackStruct->cached = true;
  ackStruct->timestamp = std::time(0);
  ackStruct->cmdTag = static_cast<Isx3CmdTag>(bytes.front());

  this->sentFramesCacheMutex.lock();
  this->sentFramesCache.push_back(ackStruct);
  if (this->sentFramesCache.size() > ACK_BUFFER_LEN) {
    // Buffer is overflowing. Indicate that the sruct at the front position is
    // no longer cached and drop the reference to it.
    this->sentFramesCache.front()->cached = false;
    this->sentFramesCache.pop_front();
  }
  this->sentFramesCacheMutex.unlock();

  // Send the bytes.
  this->sendBufferMutex.lock();
  this->sendBuffer.push_back(make_tuple(bytes, ackStruct));
  this->sendBufferMutex.unlock();

  return ackStruct;
}

bool DeviceIsx3::configure(
    std::shared_ptr<ConfigurationPayload> deviceConfiguration) {

  // Check if device is in correct state.
  if (this->deviceState != DeviceStatus::INITIALIZED) {
    LOG(WARNING) << "Could not configure ISX3 Device, as it is in state \""
                 << Device::deviceStatusToString(this->deviceState) << "\".";
    return false;
  }

  // Check if this is the correct type of configuration payload.
  std::shared_ptr<IsConfiguration> isConfiguration =
      dynamic_pointer_cast<IsConfiguration>(deviceConfiguration);
  if (!isConfiguration) {
    LOG(WARNING) << "Device ISX3 got a malformed configuration payload.";
    return false;
  }

  this->deviceState = DeviceStatus::CONFIGURING;

  // Set up the frequency point map.
  // Note: It is checked in encodeMessage() if measurementPoints is different
  // from 0.
  this->frequencyPointMap.clear();
  double frequencyIncrement =
      (isConfiguration->frequencyTo - isConfiguration->frequencyFrom) /
      isConfiguration->measurementPoints;
  frequencies.clear();
  frequencies.reserve(isConfiguration->measurementPoints);
  for (int i = 0; i < isConfiguration->measurementPoints; i++) {
    this->frequencyPointMap[i] =
        isConfiguration->frequencyFrom + frequencyIncrement * i;
    frequencies.push_back(this->frequencyPointMap[i]);
  }

  this->currentSpectrumKey =
      std::format("{:%Y%m%d%H%M}", Core::getNow()) + "_impedanceMeasurement";
  this->onConfigured(
      Utilities::KeyMapping{
          {this->currentSpectrumKey, DATAMANAGER_DATA_TYPE_SPECTRUM}},
      Utilities::SpectrumMapping{{this->currentSpectrumKey, frequencies}});

  this->configurationFinished = true;
  this->deviceState = DeviceStatus::IDLE;
  this->deviceConfiguration = deviceConfiguration;

  return true;
}

bool DeviceIsx3::waitForAck(std::shared_ptr<Isx3CmdAckStruct> ackStruct,
                            int cycles, int waitTime) {
  return false;
}

bool DeviceIsx3::start() {
  if (this->deviceState == DeviceStatus::IDLE) {
    LOG(INFO) << "ISX3 trys to start measurement.";
    LOG(INFO) << "ISX3 started measurement successfully.";
    this->deviceState = DeviceStatus::OPERATING;

    doWriteThread = true;
    writeThread.reset(new std::thread(Devices::writeThreadWorker,
                                      this->dataManager.get(), this));

    return true;
  }

  return false;
}

bool DeviceIsx3::stop() {
  if (this->deviceState == DeviceStatus::OPERATING) {
    LOG(INFO) << "ISX3 trys to stop measurement.";
    LOG(INFO) << "ISX3 stopped measurement successfully.";
    this->deviceState = DeviceStatus::IDLE;

    doWriteThread = false;
    if (writeThread) {
      writeThread->join();
    }
    writeThread.reset(nullptr);

    this->dataManager->close();

    return true;
  }

  return false;
}

bool DeviceIsx3::specificWrite(std::shared_ptr<WriteDeviceMessage> writeMsg) {
  return false;
}

std::list<std::shared_ptr<DeviceMessage>>
DeviceIsx3::specificRead(TimePoint timestamp) {
  return std::list<std::shared_ptr<DeviceMessage>>();
}

ReadPayload *DeviceIsx3::coalesceImpedanceSpectrums(
    const std::list<IsPayload> &impedanceSpectrums,
    const std::map<int, double> &frequencyPointMap) {

  if (impedanceSpectrums.empty()) {
    return nullptr;
  }

  std::list<double> frequencyValues;
  std::list<std::complex<double>> impedances;
  int channelNumber = impedanceSpectrums.front().getChannelNumber();
  double timestamp = impedanceSpectrums.front().getTimestamp();
  for (auto impedanceSpectrum : impedanceSpectrums) {
    ImpedanceSpectrum impedanceSpectrumObject =
        impedanceSpectrum.getImpedanceSpectrum();
    double frequencyPoint = get<0>(impedanceSpectrumObject.front());
    std::complex<double> impedance = get<1>(impedanceSpectrumObject.front());
    frequencyValues.push_back(
        this->frequencyPointMap[static_cast<int>(frequencyPoint)]);
    impedances.push_back(impedance);
  }

  return new IsPayload(channelNumber, timestamp, frequencyValues, impedances);
}

bool DeviceIsx3::isAcked(std::shared_ptr<Isx3CmdAckStruct> ackStruct) {
  return true;
}

bool DeviceIsx3::handleReadPayload(std::shared_ptr<ReadPayload> readPayload) {
  // Try to cast it to an impedance spectrum.
  std::shared_ptr<IsPayload> isPayload =
      dynamic_pointer_cast<IsPayload>(readPayload);
  if (isPayload) {
    // Impedance spectrum data from an ISX3 device is received one
    // frequency point at a time. Aggregate the frequency point until the
    // whole spectrum is ready to be transmitted. If a spectrum with the
    // frequency point 0 is received, it is expected that a impedance spectrum
    // has been completed, and a new one has started. Coalesce the current
    // buffer into a single spectrum, then clear it and add the current
    // impedance spectrum to it.
    if (this->impedanceSpectrumBuffer.empty() ||
        get<0>(isPayload->getImpedanceSpectrum().front()) == 0.0) {
      // Coalesce the spectrum and generate a message.
      ReadPayload *coalescedIsPayload = this->coalesceImpedanceSpectrums(
          this->impedanceSpectrumBuffer, this->frequencyPointMap);
      this->impedanceSpectrumBuffer.clear();
      IsPayload copyIsPayload = *isPayload;
      this->impedanceSpectrumBuffer.push_back(copyIsPayload);
      if (coalescedIsPayload != nullptr) {
        // Determine the destination. If responseId is set, send the messages
        // to the response id. If not, set it to the interface that sent the
        // start message.
        UserId destinationId;
        if (this->responseId.isValid()) {
          destinationId = this->responseId;
        } else {
          destinationId = this->startMessageCache->getSource();
        }

        this->pushMessageQueue(
            std::shared_ptr<DeviceMessage>(new ReadDeviceMessage(
                this->self->getUserId(), destinationId,
                ReadDeviceTopic::READ_TOPIC_DEVICE_SPECIFIC_MSG,
                coalescedIsPayload, this->startMessageCache)));
      } else {
        LOG(WARNING) << "Was not able to coalesce impedance spectrums.";
      }

    } else {
      // Timestamp did not change. Push impedance spectrum to buffer.
      IsPayload copyIsPayload = *isPayload;
      this->impedanceSpectrumBuffer.push_back(copyIsPayload);
    }

    return true;
  }

  else {
    // Uknown payload. Return false.
    return false;
  }
}

bool DeviceIsx3::initialize(std::shared_ptr<InitPayload> initPayload) {
  this->deviceState = DeviceStatus::UNKNOWN_DEVICE_STATUS;

  // Try to downcast the payload.
  std::shared_ptr<Isx3InitPayload> isx3InitPayload =
      dynamic_pointer_cast<Isx3InitPayload>(initPayload);

  if (!isx3InitPayload) {
    LOG(WARNING) << "Got malformed initialize message.";
    return false;
  }

  this->initPayload = isx3InitPayload;
  this->deviceState = DeviceStatus::INITIALIZING;

  this->onInitialized("isx3mockup", Utilities::KeyMapping(),
                      Utilities::SpectrumMapping());
  this->deviceState = DeviceStatus::INITIALIZED;

  return true;
}

bool DeviceIsx3::handleResponse(std::shared_ptr<ReadDeviceMessage> response) {
  // Device does not expect responses. Just return true here.
  return true;
}

std::string DeviceIsx3::getDeviceSerialNumber() {
  return this->deviceSerialNumber;
}

std::string DeviceIsx3::getCurrentSpectrumKey() const {
  return this->currentSpectrumKey;
}

} // namespace Devices
