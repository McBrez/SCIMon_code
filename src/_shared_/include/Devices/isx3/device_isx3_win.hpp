#ifndef DEVICE_ISX3_WIN_HPP
#define DEVICE_ISX3_WIN_HPP

// Standard includes
#include <string>

// 3rd party includes
#include "windows.h"

// Project includes
#include <device_isx3.hpp>
#include <is_configuration.hpp>

#define READ_BUFFER_SIZE 4096

namespace Devices {

class DeviceIsx3Win : public DeviceIsx3 {
public:
  /// The count of seconds it is waited on the response of the device during
  /// init.
  static const double initTimeout;

  /**
   * @brief Construct a new object.
   */
  DeviceIsx3Win();

  /**
   * @brief Closes all the handles of this object.
   */
  virtual ~DeviceIsx3Win() override;

  /**
   * Configures the device according to the given configuration.
   *
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool
  configure(shared_ptr<DeviceConfiguration> deviceConfiguration) override;

  /**
   * @brief Starts the operation of the device, provided that there is an valid
   * configuration.
   *
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool start() override;

  /**
   * @brief Stops the operation of the device.
   *
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool stop() override;

private:
  HANDLE g_hChildStd_IN_Rd = NULL;
  HANDLE g_hChildStd_IN_Wr = NULL;
  HANDLE g_hChildStd_OUT_Rd = NULL;
  HANDLE g_hChildStd_OUT_Wr = NULL;
  PROCESS_INFORMATION telnetSubprocessHandle;
  SECURITY_ATTRIBUTES saAttr;
  OVERLAPPED readPipeOverlappedStructure;

protected:
  /**
   * @brief Writes to the ISX3 device.
   *
   * @param command The command that shall be written to the device.
   * @param force If TRUE, forces the write operation. I.e. checks if the device
   * is initialized are omitted.
   * @return int The count of bytes that have been written to the device. -1
   * if an error ocurred.
   */
  virtual int writeToIsx3(const std::vector<unsigned char> &command,
                          bool force = false) override;

  virtual int readFromIsx3() override;

  virtual int initIsx3(shared_ptr<InitMessageIsx3> initMsg) override;

private:
  bool createChildProcess(const std::string &host, int port);

  int WriteToPipe(const unsigned char *buffer, unsigned int len);

  int ReadFromPipe(unsigned char *buffer, unsigned int bytesToRead);

  /**
   * @brief Builds the RESET command.
   *
   * @return The RESET command.
   */
  std::vector<unsigned char> buildCmdReset();

  /**
   * @brief Builds the GetDeviceStatus command.
   *
   * @return The GetDeviceStatus command.
   */
  std::vector<unsigned char> buildCmdGetDeviceStatus();

  std::vector<unsigned char>
  buildCmdSetupParams(shared_ptr<IsConfiguration> config);
  std::vector<unsigned char>
  buildCmdSetupParams(double minF, double maxF, IsScale logScale, int count,
                      int repetitions, string channel, double precision,
                      double amplitude, int impedanceRange, int frequencyRange);

  std::vector<unsigned char> buildCmdSendSetup();

  /**
   * The CreatePipeEx API is used to create an anonymous pipe I/O device.
   * Unlike CreatePipe FILE_FLAG_OVERLAPPED may be specified for one or
   * both handles.
   * Two handles to the device are created.  One handle is opened for
   * reading and the other is opened for writing.  These handles may be
   * used in subsequent calls to ReadFile and WriteFile to transmit data
   * through the pipe.
   * Taken from:
   * @link
   * https://stackoverflow.com/questions/60645/overlapped-i-o-on-anonymous-pipe
   *
   * @param lpReadPipe Returns a handle to the read side of the pipe. Data may
   * be read from the pipe by specifying this handle value in a subsequent call
   * to ReadFile.
   * @param lpWritePipe Returns a handle to the write side of the pipe.
   * Data may be written to the pipe by specifying this handle value in a
   * subsequent call to WriteFile.
   * @param lpPipeAttributes An optional parameter that may be used to
   * specify the attributes of the new pipe.  If the parameter is not specified,
   * then the pipe is created without a security descriptor, and the resulting
   * handles are not inherited on process creation.  Otherwise, the optional
   * security attributes are used on the pipe, and the inherit handles flag
   * effects both pipe handles.
   * @param nSize Supplies the requested buffer size for the pipe.  This is
   * only a suggestion and is used by the operating system to
   * calculate an appropriate buffering mechanism.  A value of zero
   * indicates that the system is to choose the default buffering
   * scheme.
   *
   * @return TRUE - The operation was successful. FALSE/NULL - The operation
   * failed. Extended error status is available using GetLastError.
   */
  BOOL APIENTRY MyCreatePipeEx(OUT LPHANDLE lpReadPipe,
                               OUT LPHANDLE lpWritePipe,
                               IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
                               IN DWORD nSize, DWORD dwReadMode,
                               DWORD dwWriteMode);
};
} // namespace Devices

#endif