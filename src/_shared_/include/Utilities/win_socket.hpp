#ifndef WIN_SOCKET_HPP
#define WIN_SOCKET_HPP

// Platform includes
#include <winsock2.h>
#include <ws2tcpip.h>

// Project includes
#include <socket_wrapper.hpp>

#define WIN_SOCKET_DEFAULT_BUFFER_LEN 2048

namespace Utilities {

class WinSocket : public SocketWrapper {
public:
  WinSocket();
  virtual ~WinSocket();
  virtual bool open(string ip, int port) override;
  virtual bool close() override;
  virtual int write(const vector<unsigned char> &bytes) override;
  virtual int read(vector<unsigned char> &bytes) override;
  virtual int clear() override;
  int getBufferLength();

private:
  SOCKET connectSocket;
  bool isConnected;
  char *recvbuf;
};
} // namespace Utilities

#endif