// Standard includes
#include <stdio.h>
#include <stdlib.h>

// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <win_socket.hpp>

namespace Utilities {

WinSocket::WinSocket()
    : connectSocket(INVALID_SOCKET), listenSocket(INVALID_SOCKET),
      isConnected(false), recvbuf(new char[WIN_SOCKET_DEFAULT_BUFFER_LEN]) {}

WinSocket::~WinSocket() {
  WinSocket::close();
  delete[] recvbuf;
}

bool WinSocket::listenConnection(int port) {
  if (this->isConnected) {
    LOG(WARNING)
        << "Can not start std::listening, as an connection is already open.";
    return false;
  }

  WSADATA wsaData;
  int iResult;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    LOG(ERROR) << "WSAStartup failed.";
    return false;
  }

  struct addrinfo *result;
  struct addrinfo hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the local address and port to be used by the server
  iResult = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
  if (iResult != 0) {
    LOG(ERROR) << "getaddrinfo failed: " << iResult;
    WSACleanup();
    return 1;
  }

  // Create a SOCKET for the server to listen for client connections.
  this->listenSocket =
      socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (this->listenSocket == INVALID_SOCKET) {
    LOG(ERROR) << "Creation of socket failed.";
    WSACleanup();
    return false;
  }
  // Setup the TCP listening socket
  iResult = bind(this->listenSocket, result->ai_addr, result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    LOG(ERROR) << "Bind failed with error: " << WSAGetLastError();
    closesocket(this->listenSocket);
    WSACleanup();
    return false;
  }

  iResult = listen(listenSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    LOG(ERROR) << "listen failed with error: " << WSAGetLastError();
    closesocket(this->listenSocket);
    WSACleanup();
    return false;
  }

  // Accept a client socket
  this->connectSocket = accept(this->listenSocket, NULL, NULL);
  if (this->connectSocket == INVALID_SOCKET) {
    LOG(ERROR) << "Accept failed with error: " << WSAGetLastError();
    closesocket(this->listenSocket);
    WSACleanup();
    return false;
  }

  DWORD recvTimeout = WIN_SOCKET_DEFAULT_RECV_TIMEOUT;
  setsockopt(this->connectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&recvTimeout,
             sizeof(DWORD));
  DWORD keepAlive = 1;
  setsockopt(this->connectSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&keepAlive,
             sizeof(DWORD));

  closesocket(this->listenSocket);
  this->isConnected = true;
  return true;
}

bool WinSocket::open(std::string ip, int port) {
  if (this->isConnected) {
    LOG(WARNING)
        << "Can not start connecting, as an connection is already open.";
    return false;
  }

  WSADATA wsaData;
  struct addrinfo *result = NULL, *ptr = NULL, hints;

  // Initialize Winsock
  int iResult;
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    LOG(ERROR) << "WSAStartup failed with error: " << std::to_string(iResult);
    return false;
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port.
  PCSTR ipCstr = ip.c_str();
  std::string portStr = std::to_string(port);
  PCSTR portCstr = portStr.c_str();
  iResult = getaddrinfo(ipCstr, portCstr, &hints, &result);
  if (iResult != 0) {
    LOG(ERROR) << "getaddrinfo failed with error: " << std::to_string(iResult);
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

  if (this->connectSocket == INVALID_SOCKET) {
    LOG(ERROR) << "Unable to connect to server!";
    WSACleanup();
    return false;
  }

  freeaddrinfo(result);

  DWORD recvTimeout = WIN_SOCKET_DEFAULT_RECV_TIMEOUT;
  setsockopt(this->connectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&recvTimeout,
             sizeof(DWORD));
  DWORD keepAlive = 1;
  setsockopt(this->connectSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&keepAlive,
             sizeof(DWORD));

  this->isConnected = true;
  return true;
}

bool WinSocket::close() {
  closesocket(this->listenSocket);
  closesocket(this->connectSocket);
  WSACleanup();
  this->isConnected = false;
  return true;
}

int WinSocket::write(const std::vector<unsigned char> &bytes) {
  if (!this->isConnected) {
    return -1;
  }

  const char *sendBuffer = (const char *)bytes.data();
  int iResult = send(this->connectSocket, sendBuffer, bytes.size(), 0);
  if (iResult == SOCKET_ERROR) {
    return iResult;
  }

  return iResult;
}

int WinSocket::getBufferLength() { return WIN_SOCKET_DEFAULT_BUFFER_LEN; }

int WinSocket::read(std::vector<unsigned char> &bytes) {

  if (!this->isConnected) {
    return -1;
  }

  // Clear buffer.
  ZeroMemory(this->recvbuf, this->getBufferLength());

  // Receive bytes.
  int iResult = recv(this->connectSocket, recvbuf, this->getBufferLength(), 0);
  if (iResult > 0) {
    LOG(DEBUG) << "Bytes received: " << std::to_string(iResult);
    // Append received bytes to the bytes vector.
    bytes.insert(bytes.end(), (unsigned char *)recvbuf,
                 (unsigned char *)recvbuf + iResult);
    return iResult;

  } else if (iResult == 0) {
    LOG(INFO) << "Connection closed";
    return -1;
  } else {
    int error = WSAGetLastError();
    if (WSAETIMEDOUT == error) {
      // Nothing has been received during the timeout. Just return 0.
      return 0;
    } else {
      // Some other error occured. Return error.
      LOG(INFO) << "Connection encountered an error.";
      this->isConnected = false;
      return -1;
    }
  }
}

int WinSocket::clear() {
  int bufferLen = this->getBufferLength();
  ZeroMemory(this->recvbuf, this->getBufferLength());
  return bufferLen;
}

bool WinSocket::isOpen() const { return this->isConnected; }

} // namespace Utilities
