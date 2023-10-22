// Standard includes
#include <chrono>

// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <common.hpp>
#include <data_manager.hpp>
#include <device_isx3.hpp>
#include <is_configuration.hpp>
#include <isx3_ack_payload.hpp>
#include <utilities.hpp>

namespace Devices {

DeviceIsx3::DeviceIsx3()
    : Device(DeviceType::IMPEDANCE_SPECTROMETER),
      socketWrapper(SocketWrapper::getSocketWrapper()),
      isx3CommThreadState(ISX3_COMM_THREAD_STATE_INVALID), doComm(false) {}

DeviceIsx3::~DeviceIsx3() {
  this->doComm = false;
  this->commThread->join();
  this->socketWrapper->close();
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

void DeviceIsx3::commThreadWorker() {

  while (this->doComm) {
    if (ISX3_COMM_THREAD_STATE_INVALID == this->isx3CommThreadState) {
      LOG(ERROR) << "ISX3 Communication thread transitioned into invalid "
                    "state. Communication thread is shutting down.";
      this->doComm = false;
    }

    else if (ISX3_COMM_THREAD_STATE_INIT == this->isx3CommThreadState) {
      LOG(INFO) << "Worker thread is initializing.";

      // Clear all buffers and transition into init.
      this->sendBuffer.clear();
      this->socketWrapper->clear();
      this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_LISTENING;
    }

    else if (ISX3_COMM_THREAD_STATE_LISTENING == this->isx3CommThreadState) {
      // Read from socket.
      std::vector<unsigned char> buffer;
      int readBytes = this->socketWrapper->read(buffer);
      this->commandBuffer.pushBytes(buffer);
      std::vector<unsigned char> frame = this->commandBuffer.interpretBuffer();
      std::shared_ptr<ReadPayload> decodedPayload =
          this->comInterfaceCodec.decodeMessage(frame);
      if (decodedPayload) {
        this->handleReadPayload(decodedPayload);
      }

      // If there something in the write buffer, write it to the socket.
      // Transition to ISX3_COMM_THREAD_STATE_WAITING_FOR_ACK.
      if (!this->sendBuffer.empty()) {
        this->sendBufferMutex.lock();

        std::vector<unsigned char> frame = get<0>(this->sendBuffer.front());
        this->pendingCommand = get<1>(this->sendBuffer.front());
        this->sendBuffer.pop_front();
        this->socketWrapper->write(frame);

        this->sendBufferMutex.unlock();

        this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_WAITING_FOR_ACK;
      }
    }

    else if (ISX3_COMM_THREAD_STATE_WAITING_FOR_ACK ==
             this->isx3CommThreadState) {
      // Only read from socket. If the ack has been received transition back to
      // ISX3_COMM_THREAD_STATE_LISTENING.

      // Read from socket.
      std::vector<unsigned char> buffer;
      int readBytes = this->socketWrapper->read(buffer);
      this->commandBuffer.pushBytes(buffer);
      std::vector<unsigned char> frame = this->commandBuffer.interpretBuffer();
      std::shared_ptr<ReadPayload> decodedPayload =
          this->comInterfaceCodec.decodeMessage(frame);
      if (!decodedPayload) {
        // Nothing could be decoded. Continue reading in next iteration of the
        // loop.
        continue;
      }

      // Decide what to do with the extracted payload.
      // Try to cast it to a ack payload.
      std::shared_ptr<Isx3AckPayload> ackPayload =
          dynamic_pointer_cast<Isx3AckPayload>(decodedPayload);
      if (ackPayload) {
        // This is an ack. Update the ack cache and return to std::listening.
        LOG(INFO) << "Got ACK for Command " << this->pendingCommand->cmdTag
                  << ".";
        this->pendingCommand->acked = ackPayload->getAckType();
        this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_LISTENING;
      }

      else {
        // This is not an ack. It could still be measurement data. Handle that
        // in handleReadPayload().
        bool handleSuccess = this->handleReadPayload(decodedPayload);
        if (!handleSuccess) {
          // Payload could not be handled. Transition to invalid state.
          LOG(ERROR) << "Could not handle the received payload. Worker thread "
                        "is shutting down.";
          this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_INVALID;
        }
      }
    }

    else {
      // Unknown state. Transition back to ISX3_COMM_THREAD_STATE_INVALID.
      LOG(ERROR)
          << "Unknown worker thread state. Worker thred is shutting down.";
      this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_INVALID;
    }
  }

  LOG(INFO) << "Worker thread is shutting down.";
  this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_CLOSED;
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

  // Try to parse the payload into a std::list of data frames.
  std::list<std::vector<unsigned char>> cmdList =
      this->comInterfaceCodec.encodeMessage(isConfiguration);
  if (cmdList.empty()) {
    LOG(ERROR) << "Could not parse the configuration payload into a data "
                  "frame.";
    return false;
  }

  // Send the data frames to the device and wait for the ACKs.
  bool failed = false;
  for (auto cmdFrame : cmdList) {
    std::shared_ptr<Isx3CmdAckStruct> ackStruct =
        this->pushToSendBuffer(cmdFrame);
    bool gotAck = this->waitForAck(ackStruct);
    if (!gotAck) {
      LOG(ERROR)
          << "Did not receive acknowledgment for configuration messages.";
      this->deviceState = DeviceStatus::ERROR;
      return false;
    }
  }

  // Set up the frequency point map.
  // Note: It is checked in encodeMessage() if measurementPoints is different
  // from 0.
  this->frequencyPointMap.clear();
  double frequencyIncrement =
      (isConfiguration->frequencyTo - isConfiguration->frequencyFrom) /
      isConfiguration->measurementPoints;
  for (int i = 0; i < isConfiguration->measurementPoints; i++) {
    this->frequencyPointMap[i] =
        isConfiguration->frequencyFrom + frequencyIncrement * i;
  }

  this->configurationFinished = true;
  this->deviceState = DeviceStatus::IDLE;

  return this->onConfigured(deviceConfiguration->getKeyMapping(),
                            deviceConfiguration->getSpectrumMapping());
}

bool DeviceIsx3::waitForAck(std::shared_ptr<Isx3CmdAckStruct> ackStruct,
                            int cycles, int waitTime) {
  int cycleCounter = 0;
  do {
    if (this->isAcked(ackStruct)) {
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    cycleCounter++;
  } while (cycleCounter < cycles);

  return false;
}

bool DeviceIsx3::start() {
  if (this->deviceState == DeviceStatus::IDLE) {
    LOG(INFO) << "ISX3 trys to start measurement.";

    std::shared_ptr<Isx3CmdAckStruct> ackStruct = this->pushToSendBuffer(
        this->comInterfaceCodec.buildCmdStartImpedanceMeasurement(true));
    if (this->waitForAck(ackStruct), 1000) {
      LOG(INFO) << "ISX3 started measurement successfully.";
      this->deviceState = DeviceStatus::OPERATING;

      return true;
    } else {
      LOG(ERROR) << "ISX3 could not start measurement.";
      this->deviceState = DeviceStatus::ERROR;

      return false;
    }
  } else {
    LOG(WARNING) << "Can not start ISX-3, as it is in state \""
                 << Device::deviceStatusToString(this->deviceState) << "\"";
    return false;
  }
}

bool DeviceIsx3::stop() {
  if (this->deviceState == DeviceStatus::OPERATING) {
    LOG(INFO) << "ISX3 trys to stop measurement.";

    std::shared_ptr<Isx3CmdAckStruct> ackStruct = this->pushToSendBuffer(
        this->comInterfaceCodec.buildCmdStartImpedanceMeasurement(false));
    if (this->waitForAck(ackStruct, 1000)) {
      LOG(INFO) << "ISX3 stopped measurement successfully.";
      this->deviceState = DeviceStatus::IDLE;

      return true;
    } else {
      LOG(ERROR) << "ISX3 could not stop measurement.";
      this->deviceState = DeviceStatus::ERROR;

      return false;
    }
  } else {
    LOG(WARNING) << "Can not stop ISX-3, as it is in state \""
                 << Device::deviceStatusToString(this->deviceState) << "\"";
    return false;
  }
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
  this->sentFramesCacheMutex.lock();
  auto ackStructIt = std::find(this->sentFramesCache.begin(),
                               this->sentFramesCache.end(), ackStruct);

  if (ackStructIt == this->sentFramesCache.end()) {
    this->sentFramesCacheMutex.unlock();

    return false;
  }

  if (*ackStructIt) {
    bool gotAck =
        (*ackStructIt)->acked == Isx3AckType::ISX3_ACK_TYPE_COMMAND_ACKNOWLEDGE;
    this->sentFramesCacheMutex.unlock();

    return gotAck;
  } else {
    this->sentFramesCacheMutex.unlock();

    return false;
  }
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
        // Determine the destination. If responseId is set, send the messages to
        // the response id. If not, set it to the interface that sent the start
        // message.
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

  // NOTE: The following section is blocking. It would be better to not execute
  // this in the write() function and rather handle it asyncronuously.
  // Init the connection.
  this->socketWrapper->close();
  bool openSuccess = this->socketWrapper->open(
      this->initPayload->getIpAddress(), this->initPayload->getPort());
  if (!openSuccess) {
    LOG(ERROR) << "Connection to ISX3 failed.";
    this->deviceState = DeviceStatus::ERROR;
    return false;
  }
  // Initialize thread.
  this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_INIT;
  this->doComm = true;
  this->commThread = std::unique_ptr<std::thread>(
      new std::thread(&DeviceIsx3::commThreadWorker, this));

  // Wait until communication thread is std::listening.
  int retryCounter = 0;
  bool threadInitialized = false;
  while (retryCounter < 100) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (this->isx3CommThreadState == ISX3_COMM_THREAD_STATE_LISTENING) {
      threadInitialized = true;
      break;
    }
    retryCounter++;
  }
  if (!threadInitialized) {
    // Thread was not able to initialize. Abort here.
    this->doComm = false;
    this->socketWrapper->close();
    this->deviceState = DeviceStatus::ERROR;
    LOG(ERROR) << "ISX3 COMM thread was not able to initialize.";
    return false;
  }

  // Send the init command.
  std::shared_ptr<Isx3CmdAckStruct> ackStruct =
      this->pushToSendBuffer(this->comInterfaceCodec.buildCmdSetSetup());
  // Wait for acknowledgement.
  bool positiveAck = this->waitForAck(ackStruct);
  if (positiveAck) {
    this->deviceState = DeviceStatus::INITIALIZED;

    // Init complete. Call the hook. Do not create any keys yet.
    this->onInitialized(this->getDeviceSerialNumber(), Utilities::KeyMapping(),
                        Utilities::SpectrumMapping());

    return true;
  } else {
    this->deviceState = DeviceStatus::ERROR;
    return false;
  }
}

bool DeviceIsx3::handleResponse(std::shared_ptr<ReadDeviceMessage> response) {
  // Device does not expect responses. Just return true here.
  return true;
}

std::string DeviceIsx3::getDeviceSerialNumber() {
  return this->deviceSerialNumber;
}

} // namespace Devices
