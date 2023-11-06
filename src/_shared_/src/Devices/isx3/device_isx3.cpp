// Standard includes
#include <chrono>

// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <blocking_reader.hpp>
#include <common.hpp>
#include <data_manager.hpp>
#include <device_isx3.hpp>
#include <id_payload.hpp>
#include <is_configuration.hpp>
#include <isx3_ack_payload.hpp>
#include <utilities.hpp>

namespace Devices {

DeviceIsx3::DeviceIsx3(boost::asio::io_service &io)
    : Device(DeviceType::IMPEDANCE_SPECTROMETER),
      isx3CommThreadState(ISX3_COMM_THREAD_STATE_INVALID), doComm(false),
      serialPort(nullptr), io(io) {}

DeviceIsx3::~DeviceIsx3() {

  this->io.reset();

  this->doComm = false;
  if (this->commThread && this->commThread->joinable())
    this->commThread->join();
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

  blocking_reader blockingReader(this->serialPort, 500, this->io);

  while (this->doComm) {
    if (ISX3_COMM_THREAD_STATE_INVALID == this->isx3CommThreadState) {
      LOG(ERROR) << "ISX3 Communication thread transitioned into invalid "
                    "state. Communication thread is shutting down.";
      this->doComm = false;
    }

    else if (ISX3_COMM_THREAD_STATE_INIT == this->isx3CommThreadState) {
      LOG(INFO) << "Worker thread is initializing.";
      // Clear all buffers.
      this->sendBuffer.clear();

      this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_LISTENING;

      this->commThreadInitCv.notify_all();
    }

    else if (ISX3_COMM_THREAD_STATE_LISTENING == this->isx3CommThreadState) {
      // Read from serial port.
      std::vector<unsigned char> buffer(ACK_BUFFER_LEN);
      blockingReader.read(buffer);
      // Push the bytes into the command buffer.
      this->commandBuffer.pushBytes(buffer);
      // Try to extract a frame from the command buffer.
      std::list<std::vector<unsigned char>> frames =
          this->commandBuffer.interpretBuffer();
      // Try to decode a payload from the frame.
      for (auto &frame : frames) {
        std::shared_ptr<ReadPayload> decodedPayload =
            this->comInterfaceCodec.decodeMessage(frame);
        // If a payload has been decoded, handle it accordingly.
        if (decodedPayload) {
          this->handleReadPayload(decodedPayload);
        }
      }

      // If there is something in the write buffer, write it to the serial port.
      // Transition to ISX3_COMM_THREAD_STATE_WAITING_FOR_ACK.
      if (!this->sendBuffer.empty()) {
        this->sendBufferMutex.lock();

        std::vector<unsigned char> frame = get<0>(this->sendBuffer.front());
        this->pendingCommand = get<1>(this->sendBuffer.front());
        this->sendBuffer.pop_front();
        this->serialPort->write_some(boost::asio::buffer(frame));

        this->sendBufferMutex.unlock();

        this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_WAITING_FOR_ACK;
      }
    }

    else if (ISX3_COMM_THREAD_STATE_WAITING_FOR_ACK ==
             this->isx3CommThreadState) {
      // Only read from socket. If the ack has been received transition back to
      // ISX3_COMM_THREAD_STATE_LISTENING.

      // Read from serial port ...
      std::vector<unsigned char> buffer(ACK_BUFFER_LEN);
      blockingReader.read(buffer);
      // ... push the received data into the command buffer and try to find
      // frames ...
      this->commandBuffer.pushBytes(buffer);
      std::list<std::vector<unsigned char>> frames =
          this->commandBuffer.interpretBuffer();
      // ... decode the frames ...
      for (auto &frame : frames) {
        std::shared_ptr<ReadPayload> decodedPayload =
            this->comInterfaceCodec.decodeMessage(frame);

        // Decide what to do with the extracted payload.
        // Try to cast it to a ack payload.
        std::shared_ptr<Isx3AckPayload> ackPayload =
            dynamic_pointer_cast<Isx3AckPayload>(decodedPayload);
        if (ackPayload) {
          // This is an ack. Update the ack cache and return to listening.
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
            LOG(ERROR)
                << "Could not handle the received payload. Worker thread "
                   "is shutting down.";
            this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_INVALID;
          }
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

  // Try to parse the payload into a list of data frames.
  std::list<std::vector<unsigned char>> cmdList =
      this->comInterfaceCodec.encodeMessage(isConfiguration);
  if (cmdList.empty()) {
    LOG(ERROR) << "Could not parse the configuration payload into a data "
                  "frame.";
    return false;
  }

  // Send the data frames to the device and wait for the ACKs.
  for (auto &cmdFrame : cmdList) {
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
  //  This vecotr will have the same freuencies as frequencyPointMap. It is
  //  built parallel, so the call to onConfigured() is more convenient.
  std::vector<double> frequencies;
  for (int i = 0; i < isConfiguration->measurementPoints; i++) {
    double currFreq = isConfiguration->frequencyFrom + frequencyIncrement * i;
    this->frequencyPointMap[i] = currFreq;
    frequencies.push_back(currFreq);
  }

  this->currentSpectrumKey =
      std::format("{:%Y%m%d%H%M}", Core::getNow()) + "_impedanceMeasurement";
  this->deviceState = DeviceStatus::IDLE;

  return this->onConfigured(
      Utilities::KeyMapping{
          {this->currentSpectrumKey, DATAMANAGER_DATA_TYPE_SPECTRUM}},
      Utilities::SpectrumMapping{{this->currentSpectrumKey, frequencies}});
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
    if (this->waitForAck(ackStruct, 1000)) {
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
  // Device does not support specific writes.

  return false;
}

std::list<std::shared_ptr<DeviceMessage>>
DeviceIsx3::specificRead(TimePoint timestamp) {
  // Device does not provide specific reads.
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
  for (auto &impedanceSpectrum : impedanceSpectrums) {
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
        // Determine the destination. If there are event response ids, send
        // the messages to the response ids. In any case, save the spectrum to
        // the local data manager.
        if (!this->eventResponseId.empty()) {
          for (auto &responseId : this->eventResponseId) {
            this->pushMessageQueue(
                std::shared_ptr<DeviceMessage>(new ReadDeviceMessage(
                    this->self->getUserId(), responseId,
                    ReadDeviceTopic::READ_TOPIC_DEVICE_SPECIFIC_MSG,
                    coalescedIsPayload, this->startMessageCache)));
          }
        }
        // Write the impedance spectrum.
        IsPayload *coalescedIsPayloadConcrete =
            static_cast<IsPayload *>(coalescedIsPayload);
        LOG(INFO) << "Writing spectrum to data manager ...";
        this->dataManager->write(
            Core::getNow(), this->currentSpectrumKey,
            Value(coalescedIsPayloadConcrete->getImpedanceSpectrum()));

        for (auto &impedance :
             coalescedIsPayloadConcrete->getImpedanceSpectrum()) {

          Impedance imp = std::get<Impedance>(impedance);
          double absoluteValue = std::abs(imp);
          LOG(WARNING) << std::to_string(absoluteValue);
          if (absoluteValue > 40000.0) {
            LOG(WARNING) << "Got unlikely impedance.";
          }
        }

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
    // It is not an IS payload. Is it a device id payload?
    auto idPayload = dynamic_pointer_cast<IdPayload>(readPayload);
    if (idPayload) {
      // Cache the id payload.
      this->deviceId = idPayload;

      return true;
    } else {
      // Uknown payload. Return false.
      return false;
    }
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

  this->deviceState = DeviceStatus::INITIALIZING;

  // NOTE: The following section is blocking. It would be better to not
  // execute this in the write() function and rather handle it asyncronuously.
  // Init the connection.
  LOG(INFO) << "DeviceIsx3 trys to connect to "
            << isx3InitPayload->getComPort();
  this->closeComPort();
  bool openSuccess;
  try {
    openSuccess = this->openComPort(isx3InitPayload->getComPort(),
                                    isx3InitPayload->getBaudRate());
  } catch (...) {
    LOG(ERROR) << "Connection to ISX3 failed.";
    this->deviceState = DeviceStatus::ERROR;
    return false;
  }
  if (!openSuccess) {
    LOG(ERROR) << "Connection to ISX3 failed.";
    this->deviceState = DeviceStatus::ERROR;
    return false;
  }

  LOG(INFO) << "DeviceIsx3 established a connection to "
            << isx3InitPayload->getComPort();

  // Reset and initialize the comm worker thread.
  this->doComm = false;
  if (this->commThread && this->commThread->joinable()) {
    this->commThread->join();
  }
  this->doComm = true;
  this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_INIT;
  this->commThread = std::unique_ptr<std::thread>(
      new std::thread(&DeviceIsx3::commThreadWorker, this));
  // Wait until communication thread is listening.
  std::unique_lock lk(this->commThreadInitMutex);
  std::cv_status waitResult =
      this->commThreadInitCv.wait_for(lk, std::chrono::seconds(10));
  if (waitResult == std::cv_status::timeout) {
    // comm thread did not respond within acceptable time. Return here.
    this->doComm = false;
    this->deviceState = DeviceStatus::ERROR;

    return false;
  }

  // Send a few commands to the device.
  this->pushToSendBuffer(this->comInterfaceCodec.buildCmdGetDeviceId());

  this->pushToSendBuffer(
      this->comInterfaceCodec.buildCmdStartImpedanceMeasurement(false));
  std::shared_ptr<Isx3CmdAckStruct> ackStruct =
      this->pushToSendBuffer(this->comInterfaceCodec.buildCmdSetSetup());
  // Wait for acknowledgement.
  bool positiveAck = this->waitForAck(ackStruct);
  if (positiveAck) {
    this->deviceState = DeviceStatus::INITIALIZED;

    // Init complete. Call the hook. Do not create any keys yet.
    this->onInitialized("isx3", Utilities::KeyMapping(),
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
  if (this->deviceId) {
    return std::to_string(this->deviceId->getSerialNumber());
  } else {
    return "";
  }
}

bool DeviceIsx3::openComPort(std::string comPort, int baudRate) {

  if (this->serialPort) {
    this->serialPort->close();
    this->serialPort.reset();
  }

  this->serialPort.reset(new boost::asio::serial_port(this->io, comPort));

  if (!this->serialPort->is_open()) {
    return false;
  }

  this->serialPort->set_option(
      boost::asio::serial_port_base::baud_rate(baudRate));
  this->serialPort->set_option(
      boost::asio::serial_port_base::character_size(8));
  this->serialPort->set_option(boost::asio::serial_port_base::parity(
      boost::asio::serial_port_base::parity::none));
  this->serialPort->set_option(boost::asio::serial_port_base::stop_bits(
      boost::asio::serial_port_base::stop_bits::one));

  return true;
}

bool DeviceIsx3::closeComPort() {
  if (this->serialPort && this->serialPort->is_open()) {
    this->serialPort->close();
    this->serialPort.reset();
  }

  return true;
}

} // namespace Devices
