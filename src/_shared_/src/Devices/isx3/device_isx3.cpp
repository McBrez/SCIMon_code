// Standard includes
#include <chrono>

// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <device_isx3.hpp>
#include <device_status_message.hpp>
#include <is_configuration.hpp>
#include <isx3_ack_payload.hpp>
#include <utilities.hpp>

namespace Devices {

DeviceIsx3::DeviceIsx3()
    : Device(), socketWrapper(),
      isx3CommThreadState(ISX3_COMM_THREAD_STATE_INVALID),
      pendingAck(ISX3_COMMAND_TAG_INVALID), doComm(false) {}

DeviceIsx3::~DeviceIsx3() {
  this->doComm = false;
  this->socketWrapper->close();
};

bool DeviceIsx3::write(shared_ptr<InitDeviceMessage> initMsg) {
  this->deviceState = DeviceStatus::UNKNOWN_DEVICE_STATUS;

  // Try to downcast the payload.
  shared_ptr<Isx3InitPayload> initPayload =
      dynamic_pointer_cast<Isx3InitPayload>(initMsg->returnPayload());

  if (!initPayload) {
    return false;
  }

  this->initPayload = initPayload;

  // NOTE: The following section is blocking. It would be better to not execute
  // this in the write() function and rather handle it asyncronuously.
  // Init the connection.
  this->socketWrapper->close();
  bool openSuccess = this->socketWrapper->open(
      this->initPayload->getIpAddress(), this->initPayload->getPort());
  if (!openSuccess) {
    LOG(ERROR) << "Connection to ISX3 failed.";
    return false;
  }
  // Initialize thread.
  this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_INIT;
  this->doComm = true;
  this->commThread =
      unique_ptr<thread>(new thread(&DeviceIsx3::commThreadWorker, this));

  // Wait until communication thread is listening.
  while (this->isx3CommThreadState != ISX3_COMM_THREAD_STATE_LISTENING) {
    this_thread::sleep_for(chrono::milliseconds(10));
  }

  // Send the init command.
  this->pushToSendBuffer(this->comInterfaceCodec.buildCmdSetSetup());
  this->deviceState = DeviceStatus::INIT;

  return true;
}

bool DeviceIsx3::write(shared_ptr<ConfigDeviceMessage> configMsg) {
  // Configure ISX3 according to the given data.
  return this->configure(configMsg->getConfiguration());
}

string DeviceIsx3::getDeviceTypeName() { return ""; }

void DeviceIsx3::pushToSendBuffer(const std::vector<unsigned char> &bytes) {
  this->sendBufferMutex.lock();
  this->sendBuffer.push_back(bytes);
  this->sendBufferMutex.unlock();
}

void DeviceIsx3::commThreadWorker() {

  while (this->doComm) {
    if (ISX3_COMM_THREAD_STATE_INVALID == this->isx3CommThreadState) {
      LOG(ERROR) << "ISX3 Communication thread transitioned into invalid "
                    "state. Communication thread is shutting down.";
      this->doComm = false;
    }

    else if (ISX3_COMM_THREAD_STATE_INIT == this->isx3CommThreadState) {
      // Clear all buffers and transition into init.
      this->sendBuffer.clear();
      this->socketWrapper->clear();
      this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_LISTENING;
    }

    else if (ISX3_COMM_THREAD_STATE_LISTENING == this->isx3CommThreadState) {
      // Read from socket.
      vector<unsigned char> buffer;
      int readBytes = this->socketWrapper->read(buffer);
      this->commandBuffer.pushBytes(buffer);
      vector<unsigned char> frame = this->commandBuffer.interpretBuffer();
      shared_ptr<ReadPayload> decodedPayload =
          this->comInterfaceCodec.decodeMessage(frame);

      // If there something in the write buffer, write it to the socket.
      // Transition to ISX3_COMM_THREAD_STATE_WAITING_FOR_ACK.
      if (!this->sendBuffer.empty()) {
        this->sendBufferMutex.lock();

        vector<unsigned char> frame = this->sendBuffer.front();
        this->sendBuffer.pop_front();
        this->pendingAck = static_cast<Isx3CmdTag>(frame.front());
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
      vector<unsigned char> buffer;
      int readBytes = this->socketWrapper->read(buffer);
      this->commandBuffer.pushBytes(buffer);
      vector<unsigned char> frame = this->commandBuffer.interpretBuffer();
      shared_ptr<ReadPayload> decodedPayload =
          this->comInterfaceCodec.decodeMessage(frame);

      if (!decodedPayload) {
        // Nothing could be decoded.
        continue;
      }

      // Decide what to do with the extracted payload.
      // Try to cast it to a ack payload.
      shared_ptr<Isx3AckPayload> ackPayload =
          dynamic_pointer_cast<Isx3AckPayload>(decodedPayload);
      if (ackPayload) {
        if (ackPayload->getCmdTag() == this->pendingAck)
          LOG(INFO) << "Got ACK for Command " << this->pendingAck;
        this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_LISTENING;
        this->pendingAck = ISX3_COMMAND_TAG_INVALID;
      } else {
        // Try to cast it to an impedance spectrum.
        shared_ptr<IsPayload> isPayload =
            dynamic_pointer_cast<IsPayload>(decodedPayload);
        if (isPayload) {
          // Impedance spectrum data from an ISX3 device is received one
          // frequency point at a time. Aggregate the frequency point until the
          // whole spectrum is ready to be transmitted. If the timestamp
          // changes, the spectrum is assumed as completed.
          if (this->impedanceSpectrumBuffer.back().getTimestamp() !=
              isPayload->getTimestamp()) {

            this->coalesceImpedanceSpectrums(this->impedanceSpectrumBuffer,
                                             this->frequencyPointMap);
          }
        }
      }
    }

    else {
      // Unknown state. Transition back to ISX3_COMM_THREAD_STATE_INVALID.
      this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_INVALID;
    }
  }

  this->isx3CommThreadState = ISX3_COMM_THREAD_STATE_CLOSED;
}

bool DeviceIsx3::configure(
    shared_ptr<DeviceConfiguration> deviceConfiguration) {
  return false;
}

bool DeviceIsx3::start() { return false; }

bool DeviceIsx3::stop() { return false; }

bool DeviceIsx3::specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) {
  return false;
}

list<shared_ptr<DeviceMessage>> DeviceIsx3::specificRead(TimePoint timestamp) {
  return list<shared_ptr<DeviceMessage>>();
}

shared_ptr<ReadPayload> DeviceIsx3::coalesceImpedanceSpectrums(
    const list<IsPayload> &impedanceSpectrums,
    const map<int, double> &frequencyPointMap) {

  if (impedanceSpectrums.empty()) {
    return shared_ptr<IsPayload>();
  }

  list<double> frequencyValues;
  list<std::complex<double>> impedances;
  int channelNumber = impedanceSpectrums.front().getChannelNumber();
  double timestamp = impedanceSpectrums.front().getTimestamp();
  for (auto impedanceSpectrum : impedanceSpectrums) {
    ImpedanceSpectrum impedanceSpectrumObject =
        impedanceSpectrum.getImpedanceSpectrum();
    double frequencyPoint = get<0>(impedanceSpectrumObject.front());
    std::complex<double> impedance = get<1>(impedanceSpectrumObject.front());
  }

  return shared_ptr<IsPayload>(
      new IsPayload(channelNumber, timestamp, frequencyValues, impedances));
}

} // namespace Devices
