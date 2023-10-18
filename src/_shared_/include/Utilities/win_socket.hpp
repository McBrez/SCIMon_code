#ifndef WIN_SOCKET_HPP
#define WIN_SOCKET_HPP

// Platform includes
#include <winsock2.h>
#include <ws2tcpip.h>

// Project includes
#include <socket_wrapper.hpp>

/// The default buffer length.
#define WIN_SOCKET_DEFAULT_BUFFER_LEN 2048
/// The default receive timeout duration. In milliseconds.
#define WIN_SOCKET_DEFAULT_RECV_TIMEOUT 100

namespace Utilities {
/**
 * @brief Implements windows socket communication.
 */
class WinSocket : public SocketWrapper {
public:
  /**
   * @brief Constructs a new WinSocket object.
   */
  WinSocket();

  /**
   * @brief If open, the socket is closed and destroyed.
   */
  virtual ~WinSocket() override;

  /**
   * @brief Listens on the given port for incoming connections. This method
   * blocks until a connection has been established. Call close() from another
   * thread to stop listening.
   * @param port The port that shall be listened to.
   * @return True if an connection has been established. False otherwise.
   */
  virtual bool listenConnection(int port) override;

  /**
   * @brief Trys to connect to the given ip and port.
   * @param ip The ip that shall be connected to.
   * @param port The port that shall be connected to.
   * @return TRUE if connection was succesful. FALSE otherwise.
   */
  virtual bool open(std::string ip, int port) override;

  /**
   * @brief Closes an active socket connection.
   * @return TRUE if close was succesfull. FALSE otherwise.
   */
  virtual bool close() override;

  /**
   * @brief Writes the given bytes to the socket.
   * @param bytes The bytes that are writen to the socket. The reference is used
   * as send buffer, so it has to stay valid for the enitre call of this method.
   * @return The count of bytes that have been written. A negative value
   * indicates an error.
   */
  virtual int write(const std::vector<unsigned char> &bytes) override;

  /**
   * @brief Reads bytes from the socket.
   * @param bytes Will contain the bytes that have been read.
   * @return The count of bytes that have been read from the socket. A negative
   * value indicates an error.
   */
  virtual int read(std::vector<unsigned char> &bytes) override;

  /**
   * @brief Clears the internal receive buffer.
   * @return The count of bytes that have been cleared.
   */
  virtual int clear() override;

  /**
   * @brief Returns the length of the internal receive buffer.
   * @return Length of the socket in bytes.
   */
  int getBufferLength();

  /**
   * @brief Returns whether a connection is currently open and active.
   * @return True if a connection is currently open and active.
   */
  virtual bool isOpen() const override;

private:
  /// The socket object.
  SOCKET connectSocket;
  /// The listener socket.
  SOCKET listenSocket;
  /// Flag that indicates if the socket is currently connected.
  bool isConnected;
  /// The internal receive buffer.
  char *recvbuf;
};
} // namespace Utilities

#endif
