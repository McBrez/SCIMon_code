#ifndef DEVICE_ISX3_HPP
#define DEVICE_ISX3_HPP

// 3rd party includes
#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>

// Standard includes
#include <condition_variable>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// Project includes
#include <com_interface_codec.hpp>
#include <device.hpp>
#include <id_payload.hpp>
#include <is_payload.hpp>
#include <isx3_cmd_ack_struct.hpp>
#include <isx3_command_buffer.hpp>
#include <isx3_constants.hpp>
#include <isx3_init_payload.hpp>
#include <socket_wrapper.hpp>

/// The length of the acknoledgment buffer.
#define ACK_BUFFER_LEN 1024
/// The default count of wait cycles that acknowledgments are waited for.
#define DEFAULT_ACK_WAIT_CYCLES 10
/// The default time an acknowledgment wait cycles lasts. In milliseconds.
#define DEFAULT_ACK_WAIT_DURATION 1000

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
   * @param io A reference to the io service context of boost. Is used for
   * serial port communication.
   */
  DeviceIsx3(boost::asio::io_service &io);

  /**
   * @brief Destroy the Device Isx 3 object
   */
  virtual ~DeviceIsx3() override;

  /**
   * @brief Return the name of the device type.
   * @return The device type name.
   */
  virtual std::string getDeviceTypeName() override;

  /**
   * @brief Handles a device specific write operation.
   * @param writeMsg The specific message.
   * @return True if the write operation was successfull. False otherwise.
   */
  virtual bool
  specificWrite(std::shared_ptr<WriteDeviceMessage> writeMsg) override;

  /**
   * @brief Handles a device specific read operation.
   * @param timestamp The timestamp with which this operation is called.
   * @return The response to the specific read operation.
   */
  virtual std::list<std::shared_ptr<DeviceMessage>>
  specificRead(TimePoint timestamp) override;

  /**
   * @brief Handles the response to a write message that has been sent by this
   * object.
   * @param response The response to a write message that has been sent
   * earlier.
   * @return TRUE if the response has been handled successfully. False
   * otherwise.
   */
  virtual bool
  handleResponse(std::shared_ptr<ReadDeviceMessage> response) override;

  /**
   * Configures the device according to the given configuration.
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool
  configure(std::shared_ptr<ConfigurationPayload> deviceConfiguration) override;

  /**
   * Initializes the device according to the given configuration.
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool initialize(std::shared_ptr<InitPayload> initPayload) override;

  /**
   * @brief Starts the operation of the device, provided that there is a valid
   * configuration.
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool start() override;

  /**
   * @brief Stops the operation of the device.
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool stop() override;

  /**
   * @brief Returns the serial number of the device. This is a string that
   * identifies a physical device. For instance, this can be a device serial
   * number, a static network adress, etc.
   * @return The serial number of the device.
   */
  virtual std::string getDeviceSerialNumber() override;

  /**
   * @brief Returns the key for the impedance spectrum that is currently used.
   * @return The key for the impedance spectrum that is currently used.
   */
  std::string getCurrentSpectrumKey() const;

private:
  /**
   * @brief Pushes the given COM Interface dataframe to the send buffer.
   * @param bytes The bytes that shall be pushed to the send buffer.
   * @return A reference to the sent frame.
   */
  std::shared_ptr<Isx3CmdAckStruct>
  pushToSendBuffer(const std::vector<unsigned char> &frame);

  /**
   * @brief Returns whether the given command has already been acknowledged.
   * Note that acknowledgments can go out of scope, if the acknowledgment
   * buffer (sentFramesCache) overflows.
   * @param ackStruct Reference to the acknowledgment structure, that has been
   * generated by pushToSendBuffer().
   * @return TRUE if the given cmd has been acknowledged. FALSE otherwise.
   */
  bool isAcked(std::shared_ptr<Isx3CmdAckStruct> ackStruct);

  /**
   * @brief Waits for an acknowledgment of the given command.
   * @param ackStruct The structure that identifies the command that has
   * previously been sent.
   * @param cycles Count of cycles that shall be waited for.
   * @param waitTime Specifies the length of one waiting cycle. In
   * milliseconds.
   * @return TRUE if the acknowledgment has been received during the waiting
   * time. FALSE otherwise.
   */
  bool waitForAck(std::shared_ptr<Isx3CmdAckStruct> ackStruct,
                  int cycles = DEFAULT_ACK_WAIT_CYCLES,
                  int waitTime = DEFAULT_ACK_WAIT_DURATION);

  /**
   * @brief Coalesces the given frequency point measurements into a single
   * Impedance Spectrum Measurement payload. This method is necesarry, since
   * the ISX3 device only sends back impedance values one frequency at a time.
   * Also the actual frequency value is not encoded into COM interface data
   * frame. Rather an frequency point index is given. This index has to be
   * resovled to the actual frequency values.
   * @param impedanceSpectrum std::list of impedance spectrums that only
   * contain a single freuquency point. It is expected that those impedance
   * spectrums all correspond to a single timestamp.
   * @param frequencyPointMap Mapping from frequency point index to actual
   * frequency value.
   * @return Pointer to impedance spectrum payload that contains all the
   * frequency points of impedanceSpectrums.
   */
  ReadPayload *
  coalesceImpedanceSpectrums(const std::list<IsPayload> &impedanceSpectrums,
                             const std::map<int, double> &frequencyPointMap);

  /**
   * @brief Helper function of the worker thread, that handles a read payload,
   * that has been decoded from received data from the device. This method
   * does not handle ACKs.
   * @param readPayload The payload that shall be handled.
   * @return TRUE if the payload could be handled. FALSE otherwise.
   */
  bool handleReadPayload(std::shared_ptr<ReadPayload> readPayload);

  /**
   * @brief Worker function, that handles the communication to the actual
   * device.
   */
  void commThreadWorker();

  /**
   * @brief Helper function, that opens a com port to the actual device.
   * @param comPort The comPort that shall be used.
   * @param baudRate The baudRate that shall be used.
   * @return TRUE if the connection has been established successfully. FALSE
   * otherwise.
   */
  bool openComPort(std::string comPort, int baudRate);

  /**
   * @brief Closes the com port.
   * @return TRUE if the connection has been closed successfully. FALSE
   * otherwise.
   */
  bool closeComPort();

  /// Pointer to the communication thread.
  std::unique_ptr<std::thread> commThread;

  /// Allows extraction of data frames from the data received from the socket.
  Isx3CommandBuffer commandBuffer;

  /// Allows encoding and decoding of Sciospec COM Interface frames.
  ComInterfaceCodec comInterfaceCodec;

  /// Maps from frequency point index to the actual frequency value.
  std::map<int, double> frequencyPointMap;

  /// The send frame buffer for the communication to the device. Holds the
  /// frame data (first value of the tuple), and the accompanying ack
  /// structure (second value of the tuple).
  std::list<
      std::tuple<std::vector<unsigned char>, std::shared_ptr<Isx3CmdAckStruct>>>
      sendBuffer;

  /// Mutex that guards the sendBuffer.
  std::mutex sendBufferMutex;

  /// State of the communication thread.
  Isx3CommThreadState isx3CommThreadState;

  /// Caches the command an acknowledgment is currently waited for.
  std::shared_ptr<Isx3CmdAckStruct> pendingCommand;

  /// Flag that indicates that the communication thread should do its work.
  bool doComm;

  /// Holds the received impedance spectrums until they are ready to be send.
  std::list<IsPayload> impedanceSpectrumBuffer;

  /// Caches the sent commands and whether they have been acknowledged.
  std::list<std::shared_ptr<Isx3CmdAckStruct>> sentFramesCache;

  /// Guards the acknowledgement cache.
  std::mutex sentFramesCacheMutex;

  /// The name of the current spectrum key.
  std::string currentSpectrumKey;

  /// The port that is used to communicate with the device.
  std::shared_ptr<boost::asio::serial_port> serialPort;

  /// Conditional variable, that is used when initializing the comm thread.
  std::condition_variable commThreadInitCv;

  /// A mutex that is used together with commThreadInitCv.
  std::mutex commThreadInitMutex;

  /// Caches information about the physical device.
  std::shared_ptr<IdPayload> deviceId;

  /// Reference to the boost io context. Used for serial port communication.
  boost::asio::io_service &io;
};
} // namespace Devices

#endif
