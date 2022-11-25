#include <stdio.h>
#include <strsafe.h>
#include <tchar.h>

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
  printf("\n->Start of parent execution.\n");

  // Set the bInheritHandle flag so pipe handles are inherited.

  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  // Create a pipe for the child process's STDOUT.

  if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
    ErrorExit(TEXT("StdoutRd CreatePipe"));

  // Ensure the read handle to the pipe for STDOUT is not inherited.

  if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    ErrorExit(TEXT("Stdout SetHandleInformation"));

  // Create a pipe for the child process's STDIN.

  if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
    ErrorExit(TEXT("Stdin CreatePipe"));

  // Ensure the write handle to the pipe for STDIN is not inherited.

  if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
    ErrorExit(TEXT("Stdin SetHandleInformation"));

  // Create the child process.

  this->createChildProcess();
}

void DeviceIsx3Win::createChildProcess()
// Create a child process that uses the previously created pipes for STDIN and
// STDOUT.
{
  TCHAR szCmdline[] = TEXT("child");
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  BOOL bSuccess = FALSE;

  // Set up members of the PROCESS_INFORMATION structure.

  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

  // Set up members of the STARTUPINFO structure.
  // This structure specifies the STDIN and STDOUT handles for redirection.

  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = g_hChildStd_OUT_Wr;
  siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
  siStartInfo.hStdInput = g_hChildStd_IN_Rd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  // Create the child process.

  bSuccess = CreateProcess(NULL,
                           szCmdline,    // command line
                           NULL,         // process security attributes
                           NULL,         // primary thread security attributes
                           TRUE,         // handles are inherited
                           0,            // creation flags
                           NULL,         // use parent's environment
                           NULL,         // use parent's current directory
                           &siStartInfo, // STARTUPINFO pointer
                           &piProcInfo); // receives PROCESS_INFORMATION

  // If an error occurs, exit the application.
  if (!bSuccess)
    ErrorExit(TEXT("CreateProcess"));
  else {
    // Close handles to the child process and its primary thread.
    // Some applications might keep these handles to monitor the status
    // of the child process, for example.

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    // Close handles to the stdin and stdout pipes no longer needed by the child
    // process. If they are not explicitly closed, there is no way to recognize
    // that the child process has ended.

    CloseHandle(g_hChildStd_OUT_Wr);
    CloseHandle(g_hChildStd_IN_Rd);
  }
}

void DeviceIsx3Win::WriteToPipe(void)

// Read from a file and write its contents to the pipe for the child's STDIN.
// Stop when there is no more data.
{
  DWORD dwRead, dwWritten;
  CHAR chBuf[BUFSIZE];
  BOOL bSuccess = FALSE;

  for (;;) {
    bSuccess = ReadFile(g_hInputFile, chBuf, BUFSIZE, &dwRead, NULL);
    if (!bSuccess || dwRead == 0)
      break;

    bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, dwRead, &dwWritten, NULL);
    if (!bSuccess)
      break;
  }

  // Close the pipe handle so the child process stops reading.

  if (!CloseHandle(g_hChildStd_IN_Wr))
    ErrorExit(TEXT("StdInWr CloseHandle"));
}

void DeviceIsx3Win::ReadFromPipe(void)

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT.
// Stop when there is no more data.
{
  DWORD dwRead, dwWritten;
  CHAR chBuf[BUFSIZE];
  BOOL bSuccess = FALSE;
  HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

  for (;;) {
    bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
    if (!bSuccess || dwRead == 0)
      break;

    bSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
    if (!bSuccess)
      break;
  }
}

void DeviceIsx3Win::ErrorExit(PTSTR lpszFunction)

// Format a readable error message, display a message box,
// and exit from the application.
{
  LPVOID lpMsgBuf;
  LPVOID lpDisplayBuf;
  DWORD dw = GetLastError();

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpMsgBuf, 0, NULL);

  lpDisplayBuf =
      (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) +
                                         lstrlen((LPCTSTR)lpszFunction) + 40) *
                                            sizeof(TCHAR));
  StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                  TEXT("%s failed with error %d: %s"), lpszFunction, dw,
                  lpMsgBuf);
  MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

  LocalFree(lpMsgBuf);
  LocalFree(lpDisplayBuf);
  ExitProcess(1);
}

} // namespace Devices