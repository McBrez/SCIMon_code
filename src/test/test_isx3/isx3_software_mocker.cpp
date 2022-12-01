// Standard includes
#include <string>

// 3rd party includes
#include <winsock2.h>
#include <ws2tcpip.h>

// Project includes
#include <isx3_software_mocker.hpp>

Isx3SoftwareMocker::Isx3SoftwareMocker(int port, unsigned int bufferLength)
    : port(port), socketThread(nullptr), running(false), stopFlag(false),
      bufferLength(bufferLength) {}

void Isx3SoftwareMocker::run() {
  this->running = true;
  this->socketThread = new std::thread(&Isx3SoftwareMocker::worker, this);
}

bool Isx3SoftwareMocker::isRunning() { return this->running; }

void Isx3SoftwareMocker::stop() {
  this->stopFlag = true;
  this->socketThread->join();
  delete this->socketThread;
}

void Isx3SoftwareMocker::worker() {
  WSADATA wsaData;
  int iResult;

  SOCKET ListenSocket = INVALID_SOCKET;
  SOCKET ClientSocket = INVALID_SOCKET;

  struct addrinfo *result = NULL;
  struct addrinfo hints;

  int iSendResult;
  const int recvbuflen = this->bufferLength;
  char *recvbuf = new char[recvbuflen];
  const int sendbuflen = this->bufferLength;
  char *sendbuf = new char[recvbuflen];

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    printf("WSAStartup failed with error: %d\n", iResult);
    return;
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the server address and port
  iResult =
      getaddrinfo(NULL, std::to_string(this->port).c_str(), &hints, &result);
  if (iResult != 0) {
    printf("getaddrinfo failed with error: %d\n", iResult);
    WSACleanup();
    return;
  }

  // Create a SOCKET for the server to listen for client connections.
  ListenSocket =
      socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (ListenSocket == INVALID_SOCKET) {
    printf("socket failed with error: %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return;
  }

  // Setup the TCP listening socket
  iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    printf("bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(result);
    closesocket(ListenSocket);
    WSACleanup();
    return;
  }

  freeaddrinfo(result);

  iResult = listen(ListenSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    printf("listen failed with error: %d\n", WSAGetLastError());
    closesocket(ListenSocket);
    WSACleanup();
    return;
  }

  // Accept a client socket
  ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
    printf("accept failed with error: %d\n", WSAGetLastError());
    closesocket(ListenSocket);
    WSACleanup();
    return;
  }

  // No longer need server socket
  closesocket(ListenSocket);

  // Receive until the peer shuts down the connection
  do {

    ZeroMemory(recvbuf, recvbuflen);
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
      printf("Bytes received: %d\n", iResult);
      printf("Received the following: %s\n", recvbuf);

      std::string received(recvbuf);

      int pos = received.find("getDeviceStatus");
      if (pos >= 0) {
        std::string sendBuffer = "deviceStatus idle";
        const char *charBuffer = sendBuffer.c_str();
        iSendResult = send(ClientSocket, charBuffer, iResult, 0);
      }

      // Echo the buffer back to the sender
      iSendResult = send(ClientSocket, recvbuf, iResult, 0);
      if (iSendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return;
      }
      printf("Bytes sent: %d\n", iSendResult);
    } else {
      printf("recv failed with error: %d\n", WSAGetLastError());
      closesocket(ClientSocket);
      WSACleanup();
      return;
    }

  } while (iResult > 0 && !this->stopFlag);

  // shutdown the connection since we're done
  iResult = shutdown(ClientSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(ClientSocket);
    WSACleanup();
    return;
  }

  // cleanup
  closesocket(ClientSocket);
  WSACleanup();
  delete[] recvbuf;

  return;
}
