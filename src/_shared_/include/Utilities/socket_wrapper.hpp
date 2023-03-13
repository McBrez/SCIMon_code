#ifndef SOCKET_WRAPPER_HPP
#define SOCKET_WRAPPER_HPP

// Standard includes
#include <string>
#include <vector>

using namespace std;

namespace Utilities {
/**
 * @brief Defines an interface to a class that can read from sockets.
 */
class SocketWrapper {
public:
  virtual ~SocketWrapper() = 0;
  virtual bool open(string ip, int port) = 0;
  virtual bool close() = 0;
  virtual int write(const vector<unsigned char> &bytes) = 0;
  virtual int read(vector<unsigned char> &bytes) = 0;
  virtual int clear() = 0;
};

} // namespace Utilities

#endif