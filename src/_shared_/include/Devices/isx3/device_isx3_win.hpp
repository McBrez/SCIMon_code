#ifndef DEVICE_ISX3_WIN_HPP
#define DEVICE_ISX3_WIN_HPP

// 3rd party includes
#include "windows.h"

// Project includes
#include <device_isx3.hpp>

#define READ_BUFFER_SIZE 254
#define BUFSIZE 4096

namespace Devices {
class DeviceIsx3Win : public DeviceIsx3 {
public:
  DeviceIsx3Win();

private:
  /// Reference to a handle to the device.
  HANDLE winHandle;

  HANDLE g_hChildStd_IN_Rd = NULL;
  HANDLE g_hChildStd_IN_Wr = NULL;
  HANDLE g_hChildStd_OUT_Rd = NULL;
  HANDLE g_hChildStd_OUT_Wr = NULL;
  HANDLE g_hInputFile = NULL;
  SECURITY_ATTRIBUTES saAttr;

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

  virtual int initIsx3() override;

private:
  void createChildProcess();

  void WriteToPipe(void);

  void ReadFromPipe(void);

  void ErrorExit(PTSTR lpszFunction);
};
} // namespace Devices

#endif