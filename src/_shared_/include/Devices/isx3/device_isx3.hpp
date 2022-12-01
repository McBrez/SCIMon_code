#ifndef DEVICE_ISX3_HPP
#define DEVICE_ISX3_HPP

// Standard includes
#include <vector>

// Project includes
#include <device.hpp>
#include <init_message_isx3.hpp>
#include <isx3_constants.hpp>

namespace Devices {
class DeviceIsx3 : public Device {
public:
  DeviceIsx3();
  virtual ~DeviceIsx3() = 0;

  /**
   * @brief Return the name of the device type.
   *
   * @return The device type name.
   */
  virtual string getDeviceTypeName() override;

  bool isConfigured();
  virtual bool write(shared_ptr<InitDeviceMessage> initMsg) override;
  virtual bool write(shared_ptr<WriteDeviceMessage> writeMsg) override;
  virtual bool write(shared_ptr<ConfigDeviceMessage> configMsg) override;
  virtual shared_ptr<ReadDeviceMessage> read() override;

private:
  /// Vector of known command tags.
  static const std::vector<unsigned char> knownCommandTags;

protected:
  /// Buffer for read operations to the device.
  std::vector<unsigned char> readBuffer;

  /**
   * @brief Interprets the buffer and generates corresponding messages.
   *
   * @return A message that could be extracted from the read buffer. Empty
   * pointer, if no message could be retrieved.
   */
  shared_ptr<ReadDeviceMessage>
  interpretBuffer(std::vector<unsigned char> &readBuffer);

  /**
   * The OS-specific part of the write operation.
   *
   * @param command The command that shall be written to the device.
   * @param force If TRUE, forces the write operation. I.e. checks if the device
   * is initialized are omitted.
   * @return int The count of bytes that have been written to the device. -1
   * if an error ocurred.
   */
  virtual int writeToIsx3(const std::vector<unsigned char> &command,
                          bool force = false) = 0;

  virtual int readFromIsx3() = 0;

  /**
   * The OS-specific part of the initialization. Is called, when an
   * initialization message has been received.
   *
   * @param initMsg The init message that triggered this call.
   * @return 0 If no error occured. Error code otherwise.
   */
  virtual int initIsx3(shared_ptr<InitMessageIsx3> initMsg) = 0;
};
} // namespace Devices

#endif