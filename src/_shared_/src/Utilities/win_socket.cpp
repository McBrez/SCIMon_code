// Platform includes
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <win_socket.hpp>

namespace Utilities {

WinSocket::WinSocket()
    : connectSocket(INVALID_SOCKET), isConnected(false),
      recvbuf(new char[WIN_SOCKET_DEFAULT_BUFFER_LEN]) {}

WinSocket::~WinSocket() {
  this->close();
  delete[] recvbuf;
}

bool WinSocket::open(string ip, int port) {
  WSADATA wsaData;
  struct addrinfo *result = NULL, *ptr = NULL, hints;

  int iResult;
  int recvbuflen = WIN_SOCKET_DEFAULT_BUFFER_LEN;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    LOG(ERROR) << "WSAStartup failed with error: " << to_string(iResult);
    return false;
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port.
  PCSTR ipCstr = ip.c_str();
  string portStr = to_string(port);
  PCSTR portCstr = portStr.c_str();
  iResult = getaddrinfo(ipCstr, portCstr, &hints, &result);
  if (iResult != 0) {
    LOG(ERROR) << "getaddrinfo failed with error: " << to_string(iResult);
    WSACleanup();
    return false;
  }

  // Attempt to connect to an address until one succeeds
  for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

    // Create a SOCKET for connecting to server
    this->connectSocket =
        socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (this->connectSocket == INVALID_SOCKET) {
      LOG(ERROR) << "Socket could not be created.";
      WSACleanup();
      return false;
    }

    // Connect to server.
    iResult = connect(this->connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
      LOG(ERROR) << "Socket could not be created.";
      closesocket(this->connectSocket);
      this->connectSocket = INVALID_SOCKET;
      continue;
    }
    break;
  }

  freeaddrinfo(result);
  DWORD recvTimeout = WIN_SOCKET_DEFAULT_RECV_TIMEOUT;
  setsockopt(this->connectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&recvTimeout,
             sizeof(DWORD));
  if (this->connectSocket == INVALID_SOCKET) {
    LOG(ERROR) << "Unable to connect to server!";
    WSACleanup();
    return false;
  }

  this->isConnected = true;
  return true;
}

bool WinSocket::close() {
  if (!this->isConnected) {
    return false;
  }

  closesocket(this->connectSocket);
  WSACleanup();
  this->isConnected = false;
  return true;
}

int WinSocket::write(const vector<unsigned char> &bytes) {
  if (!this->isConnected) {
    return false;
  }

  const char *sendBuffer = (const char *)bytes.data();
  int iResult = send(this->connectSocket, sendBuffer, bytes.size(), 0);
  if (iResult == SOCKET_ERROR) {

    return false;
  }

  return true;
}

int WinSocket::getBufferLength() { return WIN_SOCKET_DEFAULT_BUFFER_LEN; }

int WinSocket::read(vector<unsigned char> &bytes) {

  if (!this->isConnected) {
    return false;
  }

  // Clear buffer.
  ZeroMemory(this->recvbuf, this->getBufferLength());

  // Receive bytes.
  int iResult = recv(this->connectSocket, recvbuf, this->getBufferLength(), 0);
  if (iResult > 0) {
    LOG(INFO) << "Bytes received: " << to_string(iResult);
    bytes =
        vector((unsigned char *)recvbuf, (unsigned char *)recvbuf + iResult);
    return iResult;

  } else if (iResult == 0) {
    LOG(INFO) << "Connection closed";
    return 0;
  } else {
    return -1;
  }
}

int WinSocket::clear() {
  int bufferLen = this->getBufferLength();
  ZeroMemory(this->recvbuf, this->getBufferLength());
  return bufferLen;
}

} // namespace Utilities