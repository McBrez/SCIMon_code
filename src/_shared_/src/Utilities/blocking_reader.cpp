// 3rd party includes
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/bind.hpp>

// Project includes
#include <blocking_reader.hpp>

void blocking_reader::read_complete(const boost::system::error_code &error,
                                    size_t bytes_transferred) {
  read_error = error.failed();
  // Read has finished, so cancel the
  // timer.
  timer.cancel();
}

// Called when the timer's deadline expires.
void blocking_reader::time_out(const boost::system::error_code &error) {
  // Was the timeout was cancelled?
  if (error) {
    // yes
    return;
  }
  // no, we have timed out, so kill
  // the read operation
  // The read callback will be called
  // with an error
  port->cancel();
}

blocking_reader::blocking_reader(std::shared_ptr<boost::asio::serial_port> port,
                                 size_t timeout, boost::asio::io_service &io)
    : port(port), timeout(timeout), timer(io), io(io), read_error(true) {}

// Reads a character or times out
// returns false if the read times out
bool blocking_reader::read(std::vector<unsigned char> &buffer) {
  // After a timeout & cancel it seems we need
  // to do a reset for subsequent reads to work.
  this->io.reset();
  // Asynchronously read 1 character.
  boost::asio::async_read(
      *port, boost::asio::buffer(buffer),
      boost::bind(&blocking_reader::read_complete, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
  // Setup a deadline time to implement our timeout.
  timer.expires_from_now(boost::posix_time::milliseconds(timeout));
  timer.async_wait(boost::bind(&blocking_reader::time_out, this,
                               boost::asio::placeholders::error));
  // This will block until a character is read
  // or until the it is cancelled.
  io.run();
  return !read_error;
}
