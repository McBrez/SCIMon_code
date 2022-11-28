#ifndef ISX3_SOFTWARE_MOCKER_HPP
#define ISX3_SOFTWARE_MOCKER_HPP

#include <thread>

/**
 * Mocks the ISX3 server software, by accepting a socket connection at the given
 * port.
 */
class Isx3SoftwareMocker {

public:
  /**
   * @brief Construct a new ISX3 Software Mocker object
   * @param port The port at which shall be listened.
   * @param bufferLength The length of the socket buffer.
   */
  Isx3SoftwareMocker(int port, unsigned int bufferLength = 4096);

  /**
   * @brief Sets up the socket in a seperate thread and starts listening.
   */
  void run();

  /**
   * Closes the socket and terminates the thread.
   */
  void stop();

  /**
   * @brief Contains the logic for the socket.
   */
  void worker();

  /**
   * @brief Whether the thread is running or not.
   * @return True if the thread is running. False otherwise.
   */
  bool isRunning();

private:
  /// The thread that handles the socket execution.
  std::thread *socketThread;
  /// The port on which to listen.
  int port;
  /// Flag that inidicates, if the thread is running.
  bool running;
  /// A flag that indicates to the managed thread to stop.
  bool stopFlag;
  /// The buffer length for the socket.
  unsigned int bufferLength;
};

#endif