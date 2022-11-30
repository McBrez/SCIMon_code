// Standard includes
#include <chrono>
#include <ctime>
#include <format>
#include <stdio.h>
#include <strsafe.h>
#include <thread>

// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <device_isx3_win.hpp>
#include <device_status_message.hpp>

namespace Devices {

const std::string DeviceIsx3Win::telnetExecutableName =
    "plink -telnet -P {0} {1}";

const std::string DeviceIsx3Win::cmdTemplateReset = "reset\n";

const std::string DeviceIsx3Win::cmdTemplateGetDeviceStatus =
    "getDeviceStatus\n";

// {0} ... start frequency
// {1} ... end frequency
// {2} ... scale; 0 for linear, 1 for logarithmic
// {3} ... count of measurement points
// {4} ... count of measurements; 0 for continuous measurement
// {5} ... channel; BNC, MEA or EXTMUX32
// {6} ... precision
// {7} ... aimplitude in mV
// {8} ... impedance range; 0 for kilo ohm, 1 for mega ohm
// {9} ... frequency resolution; 1 for <100kHz, 2 for <1Mhz, 3 for <10Mhz
const std::string DeviceIsx3Win::cmdTemplateSetupParams =
    "setupParams {0},{1},{2},{3},{4},{5},{6},{7},{8},{9}\n";

const double DeviceIsx3Win::initTimeout = 10;

DeviceIsx3Win::DeviceIsx3Win() {
  // Set up members of the PROCESS_INFORMATION structure.
  ZeroMemory(&this->telnetSubprocessHandle, sizeof(PROCESS_INFORMATION));
  // Set up members of the OVERLAPPED structure.
  ZeroMemory(&this->readPipeOverlappedStructure, sizeof(OVERLAPPED));
}

DeviceIsx3Win::~DeviceIsx3Win() {

  CloseHandle(this->g_hChildStd_IN_Wr);
  CloseHandle(this->g_hChildStd_OUT_Rd);
  // Close handles to the child process and its primary thread.
  // Some applications might keep these handles to monitor the status
  // of the child process, for example.
  CloseHandle(this->telnetSubprocessHandle.hProcess);
  CloseHandle(this->telnetSubprocessHandle.hThread);
}

int DeviceIsx3Win::writeToIsx3(const std::vector<unsigned char> &command,
                               bool force) {
  // Is the device already initialized?
  if (!force && !this->isInitialized()) {
    LOG(ERROR) << "Requested write on an uninitialized device. Request will be "
                  "ignored.";
    return -1;
  }

  int bytesWritten = this->WriteToPipe(command.data(), command.size());
  if (bytesWritten == -1) {
    return -1;
  }

  return bytesWritten;
}

int DeviceIsx3Win::readFromIsx3() {
  unsigned char *charReadBuffer = new unsigned char[READ_BUFFER_SIZE];
  memset(charReadBuffer, 0, READ_BUFFER_SIZE);

  int bytesRead = this->ReadFromPipe(charReadBuffer, READ_BUFFER_SIZE);

  if (bytesRead == -1) {
    return -1;
  }

  for (int i = 0; i < bytesRead; i++) {
    this->readBuffer.push_back(charReadBuffer[i]);
  }

  delete[] charReadBuffer;
  return bytesRead;
}

int DeviceIsx3Win::initIsx3(shared_ptr<InitMessageIsx3> initMsg) {
  // Set the bInheritHandle flag so pipe handles are inherited.
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  // Create a pipe for the child process's STDOUT. The pipe shall be overlapped
  // in order to allow asynchronous read operations.
  if (!this->MyCreatePipeEx(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr,
                            0, FILE_FLAG_OVERLAPPED, 0)) {
    return -1;
  }
  // Set a timeout on the pipe, so that read operations do not block, when there
  // is no data.
  COMMTIMEOUTS timeoutStructure;
  timeoutStructure.ReadIntervalTimeout = 10;
  timeoutStructure.ReadTotalTimeoutMultiplier = 0;
  timeoutStructure.ReadTotalTimeoutConstant = 10;
  timeoutStructure.WriteTotalTimeoutMultiplier = 0;
  timeoutStructure.WriteTotalTimeoutConstant = 10;
  SetCommTimeouts(&g_hChildStd_OUT_Rd, &timeoutStructure);

  // Ensure the read handle to the pipe for STDOUT is not inherited.
  if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
    return -1;
  }

  // Create a pipe for the child process's STDIN.
  if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) {
    return -1;
  }

  // Ensure the write handle to the pipe for STDIN is not inherited.
  if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
    return -1;
  }

  // Create the child process.
  bool success = this->createChildProcess(initMsg->getTelnetHostName(),
                                          initMsg->getTelnetPort());
  if (!success) {
    return -1;
  }

  // Send reset command to ISX3.
  this->writeToIsx3(this->buildCmdReset(), true);
  // Check if the device is ready.
  this->writeToIsx3(this->buildCmdGetDeviceStatus(), true);
  // Try to read from device, until a response has been received or until a
  // timeout has elapsed.
  std::time_t start = std::time(0);
  shared_ptr<DeviceMessage> response;
  double timeDiff = 0;
  LOG(DEBUG) << "Waiting on response from ISX3 after INIT ...";
  do {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    this->readFromIsx3();
    response = this->interpretBuffer(this->readBuffer);
    timeDiff = std::difftime(std::time(NULL), start);

  } while (!response && timeDiff < DeviceIsx3Win::initTimeout);

  // Has a response arrived?
  if (response) {
    LOG(DEBUG) << "Got response from ISX3.";
    // A response has arrived. Check if it has the expected content.
    shared_ptr<DeviceStatusMessage> statusMsg =
        dynamic_pointer_cast<DeviceStatusMessage>(response);
    if (!statusMsg) {
      // Not the expected response. Return here.
      return -1;
    }
    if (statusMsg->getDeviceStatus() == DeviceStatus::IDLE) {
      // This is the expected response. The device has been initialized
      // succesfully. Return accordingly.
      this->initFinished = true;
      return 0;
    } else {
      // Device is not in IDLE. This is not the expected response. Return here.
      return -1;
    }
  } else {
    // Init failed. Return here.
    LOG(DEBUG) << "Got no response from ISX3.";
    return -1;
  }
}

bool DeviceIsx3Win::createChildProcess(const std::string &host, int port) {

  // Construct the command line.
  std::string_view commandLineView = DeviceIsx3Win::telnetExecutableName;
  std::string commandLine =
      std::vformat(commandLineView, std::make_format_args(port, host));
  char *commandLineCStr = new char[commandLine.length() + 1];
  strcpy_s(commandLineCStr, commandLine.length() + 1, commandLine.c_str());

  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  BOOL bSuccess = FALSE;

  // Set up members of the STARTUPINFO structure.
  // This structure specifies the STDIN and STDOUT handles for redirection.
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = g_hChildStd_OUT_Wr;
  siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
  siStartInfo.hStdInput = g_hChildStd_IN_Rd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  // Create the child process.
  bSuccess = CreateProcess(
      NULL,
      commandLineCStr,                // command line
      NULL,                           // process security attributes
      NULL,                           // primary thread security attributes
      TRUE,                           // handles are inherited
      0,                              // creation flags
      NULL,                           // use parent's environment
      NULL,                           // use parent's current directory
      &siStartInfo,                   // STARTUPINFO pointer
      &this->telnetSubprocessHandle); // receives PROCESS_INFORMATION

  // Delete command line c string as it is no longer needed.
  delete[] commandLineCStr;

  // If an error occurs, exit the application.
  if (!bSuccess)
    return false;
  else {
    // Close handles to the stdin and stdout pipes no longer needed by the child
    // process. If they are not explicitly closed, there is no way to recognize
    // that the child process has ended.
    CloseHandle(g_hChildStd_OUT_Wr);
    CloseHandle(g_hChildStd_IN_Rd);

    return true;
  }
}

int DeviceIsx3Win::WriteToPipe(const unsigned char *buffer, unsigned int len) {
  DWORD dwWritten;
  BOOL bSuccess = FALSE;

  bSuccess = WriteFile(g_hChildStd_IN_Wr, buffer, len, &dwWritten, NULL);

  if (bSuccess) {
    return dwWritten;
  } else {
    return -1;
  }
}

int DeviceIsx3Win::ReadFromPipe(unsigned char *buffer,
                                unsigned int bytesToRead) {
  DWORD bytesRead;
  BOOL success = FALSE;

  success = ReadFile(g_hChildStd_OUT_Rd, buffer, bytesToRead, &bytesRead,
                     &this->readPipeOverlappedStructure);
  if (!success) {
    return -1;
  }
  return bytesRead;
}

std::vector<unsigned char> DeviceIsx3Win::buildCmdReset() {
  std::vector<unsigned char> retVal;
  retVal.reserve(DeviceIsx3Win::cmdTemplateReset.length());

  for (unsigned char ch : DeviceIsx3Win::cmdTemplateReset) {
    retVal.push_back(ch);
  }
  return retVal;
}

std::vector<unsigned char> DeviceIsx3Win::buildCmdGetDeviceStatus() {
  std::vector<unsigned char> retVal;
  retVal.reserve(DeviceIsx3Win::cmdTemplateGetDeviceStatus.length());

  for (unsigned char ch : DeviceIsx3Win::cmdTemplateGetDeviceStatus) {
    retVal.push_back(ch);
  }
  return retVal;
}

BOOL DeviceIsx3Win::MyCreatePipeEx(OUT LPHANDLE lpReadPipe,
                                   OUT LPHANDLE lpWritePipe,
                                   IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
                                   IN DWORD nSize, DWORD dwReadMode,
                                   DWORD dwWriteMode) {
  HANDLE ReadPipeHandle, WritePipeHandle;
  DWORD dwError;
  CHAR PipeNameBuffer[MAX_PATH];
  static volatile long PipeSerialNumber;

  //
  // Only one valid OpenMode flag - FILE_FLAG_OVERLAPPED
  //

  if ((dwReadMode | dwWriteMode) & (~FILE_FLAG_OVERLAPPED)) {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  //
  //  Set the default timeout to 120 seconds
  //

  if (nSize == 0) {
    nSize = 4096;
  }

  sprintf(PipeNameBuffer, "\\\\.\\Pipe\\RemoteExeAnon.%08x.%08x",
          GetCurrentProcessId(), InterlockedIncrement(&PipeSerialNumber));

  ReadPipeHandle =
      CreateNamedPipeA(PipeNameBuffer, PIPE_ACCESS_INBOUND | dwReadMode,
                       PIPE_TYPE_BYTE | PIPE_WAIT,
                       1,     // Number of pipes
                       nSize, // Out buffer size
                       nSize, // In buffer size
                       0, lpPipeAttributes);

  if (!ReadPipeHandle) {
    return FALSE;
  }

  WritePipeHandle = CreateFileA(PipeNameBuffer, GENERIC_WRITE,
                                0, // No sharing
                                lpPipeAttributes, OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL | dwWriteMode,
                                NULL // Template file
  );

  if (INVALID_HANDLE_VALUE == WritePipeHandle) {
    dwError = GetLastError();
    CloseHandle(ReadPipeHandle);
    SetLastError(dwError);
    return FALSE;
  }

  *lpReadPipe = ReadPipeHandle;
  *lpWritePipe = WritePipeHandle;
  return (TRUE);
}

} // namespace Devices