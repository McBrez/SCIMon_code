#ifndef DEVICE_ISX3_HPP
#define DEVICE_ISX3_HPP

// Standard includes
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// Project includes
#include <com_interface_codec.hpp>
#include <device.hpp>
#include <is_payload.hpp>
#include <isx3_command_buffer.hpp>
#include <isx3_constants.hpp>
#include <isx3_init_payload.hpp>
#include <socket_wrapper.hpp>

using namespace Utilities;

namespace Devices {

enum Isx3CommThreadState {
  ISX3_COMM_THREAD_STATE_INVALID,
  ISX3_COMM_THREAD_STATE_INIT,
  ISX3_COMM_THREAD_STATE_LISTENING,
  ISX3_COMM_THREAD_STATE_WAITING_FOR_ACK,
  ISX3_COMM_THREAD_STATE_CLOSED,
};

/**
 * @brief Depcits an Sciosepc ISX3 device.
 */
class DeviceIsx3 : public Device {
public:
  /**
   * @brief Construct a new Device Isx 3 object
   */
  DeviceIsx3();

  /**
   * @brief Destroy the Device Isx 3 object
   */
  virtual ~DeviceIsx3() override;

  /**
   * @brief Return the name of the device type.
   * @return The device type name.
   */
  virtual string getDeviceTypeName() override;

  virtual bool write(shared_ptr<InitDeviceMessage> initMsg) override;

  virtual bool write(shared_ptr<ConfigDeviceMessage> configMsg) override;

  virtual bool specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) override;

  virtual list<shared_ptr<DeviceMessage>>
  specificRead(TimePoint timestamp) override;

  /**
   * Configures the device according to the given configuration.
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool
  configure(shared_ptr<DeviceConfiguration> deviceConfiguration) override;

  /**
   * @brief Starts the operation of the device, provided that there is an valid
   * configuration.
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool start() override;

  /**
   * @brief Stops the operation of the device.
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool stop() override;

private:
  /**
   * @brief Worker function for the communication thread.
   */
  void commThreadWorker();

  /**
   * @brief Pushes the given COM Interface dataframe to the send buffer.
   * @param bytes The bytes that shall be pushed to the send buffer.
   */
  void pushToSendBuffer(const std::vector<unsigned char> &frame);

  /// Pointer to the communication thread.
  unique_ptr<thread> commThread;

  /// Pointer to a wrapper that provides platform independent socket
  /// functionality.
  unique_ptr<SocketWrapper> socketWrapper;

  /// Allows extraction of data frames from the data received from the socket.
  Isx3CommandBuffer commandBuffer;

  /// Allows encoding and decoding of Sciospec COM Interface frames.
  ComInterfaceCodec comInterfaceCodec;

  /// Maps from frequency point index to the actual frequency value.
  map<int, double> frequencyPointMap;

  /// The init payload with which the device is initialized.
  shared_ptr<Isx3InitPayload> initPayload;

  /// The send frame buffer for the communication to the device.
  list<vector<unsigned char>> sendBuffer;

  /// Mutex that guards the sendBuffer.
  mutex sendBufferMutex;

  /// State of the communication thread.
  Isx3CommThreadState isx3CommThreadState;

  /// Caches the type of acknowledgment message, that is waited for.
  Isx3CmdTag pendingAck;

  /// Flag that indicates that the communication thread should do its work.
  bool doComm;

  /// Holds the received impedance spectrums until they are ready to be send.
  list<IsPayload> impedanceSpectrumBuffer;
};
} // namespace Devices

#endif
