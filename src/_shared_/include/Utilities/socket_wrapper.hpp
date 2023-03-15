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
  /**
   * @brief Destroy the Socket Wrapper object
   */
  virtual ~SocketWrapper() = 0;

  /**
   * @brief Trys to connect to the given ip and port.
   * @param ip The ip that shall be connected to.
   * @param port The port that shall be connected to.
   * @return TRUE if connection was succesful. FALSE otherwise.
   */
  virtual bool open(string ip, int port) = 0;

  /**
   * @brief Closes an active socket connection.
   * @return TRUE if close was succesfull. FALSE otherwise.
   */
  virtual bool close() = 0;

  /**
   * @brief Writes the given bytes to the socket.
   * @param bytes The bytes that are writen to the socket. The reference is used
   * as send buffer, so it has to stay valid for the enitre call of this method.
   * @return The count of bytes that have been written. A negative value
   * indicates an error.
   */
  virtual int write(const vector<unsigned char> &bytes) = 0;

  /**
   * @brief Reads bytes from the socket.
   * @param bytes Will contain the bytes that have been read.
   * @return The count of bytes that have been read from the socket. A negative
   * value indicates an error.
   */
  virtual int read(vector<unsigned char> &bytes) = 0;

  /**
   * @brief Clears the internal receive buffer.
   * @return The count of bytes that have been cleared.
   */
  virtual int clear() = 0;
};

} // namespace Utilities

#endif