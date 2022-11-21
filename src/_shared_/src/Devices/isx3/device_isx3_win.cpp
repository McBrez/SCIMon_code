// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <device_isx3_win.hpp>

namespace Devices {
DeviceIsx3Win::DeviceIsx3Win() : winHandle(nullptr) {}

int DeviceIsx3Win::writeToIsx3(const std::vector<unsigned char> &command,
                               bool force) {
  // Is the device already initialized?
  if (!force && !this->isInitialized()) {
    LOG(ERROR) << "Requested write on an uninitialized device. Request will be "
                  "ignored.";
    return -1;
  }

  int dataCount = command.size();
  char *buffer = new char[dataCount];
  for (int i = 0; i < dataCount; i++) {
    buffer[i] = command[i];
  }

  unsigned long bytesWritten;

  auto Status = WriteFile(this->winHandle, // Handle to the Serial port
                          buffer,          // Data to be written to the port
                          dataCount,       // No of bytes to write
                          &bytesWritten,   // Bytes written
                          NULL);

  // FT_STATUS status = FT_Write(*this->handle, buffer, dataCount,
  // &bytesWritten);

  LOG(DEBUG) << "Wrote " << bytesWritten
             << " bytes to ISX3 with status: " << Status;

  delete[] buffer;

  return bytesWritten == dataCount ? bytesWritten : -1;
}

int DeviceIsx3Win::readFromIsx3() {
  unsigned char *readBuffer = new unsigned char[READ_BUFFER_SIZE];
  DWORD bytesRead = 0;
  bool status = ReadFile(this->winHandle, readBuffer, 254, &bytesRead, nullptr);

  for (int i = 0; i < bytesRead; i++) {
    this->readBuffer.push_back(readBuffer[i]);
  }
  delete[] readBuffer;

  return bytesRead;
}

int DeviceIsx3Win::initIsx3() {
  this->winHandle = CreateFileA("\\\\.\\COM7",                // port name
                                GENERIC_READ | GENERIC_WRITE, // Read/Write
                                0,                            // No Sharing
                                NULL,                         // No Security
                                OPEN_EXISTING, // Open existing port only
                                0,             // Non Overlapped I/O
                                NULL);         // Null for Comm Devices

  if (this->winHandle == INVALID_HANDLE_VALUE)
    printf("Error in opening serial port");
  else
    printf("opening serial port successful");

  DCB dcb;
  GetCommState(this->winHandle, &dcb);
  dcb.BaudRate = 768000;
  dcb.EofChar = 0x0;
  dcb.ErrorChar = 0x0;
  dcb.Parity = 0;
  dcb.fParity = 0;
  dcb.fOutxCtsFlow = true;
  dcb.fOutxDsrFlow = true;
  dcb.XonChar = 0x11;
  dcb.XoffChar = 0x13;
  dcb.StopBits = 0;
  dcb.XonLim = 16;
  dcb.XoffLim = 1024;
  dcb.fDtrControl = 0x02;
  dcb.fInX = true;
  dcb.fOutX = true;
  dcb.fRtsControl = 0x02;

  bool setCommStateStatus = SetCommState(this->winHandle, &dcb);

  return this->winHandle != nullptr && setCommStateStatus;
}

} // namespace Devices