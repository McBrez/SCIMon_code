// Project includes
#include <socket_wrapper.hpp>
// Socket Wrapper implementations.
#include <win_socket.hpp>

namespace Utilities {

SocketWrapper::~SocketWrapper() {}

SocketWrapper *SocketWrapper::getSocketWrapper() {
#if WIN32
  return new WinSocket();
#else
#error Other platform than WIN32 are currently not supported.
#endif
}
} // namespace Utilities
