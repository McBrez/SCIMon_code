#ifndef BLOCKING_READER_HPP
#define BLOCKING_READER_HPP

//   Copyright 2012 Kevin Godden
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
// blocking_reader.h - a class that provides basic support for
// blocking & time-outable single character reads from
// boost::asio::serial_port.
//
// use like this:
//
// 	blocking_reader reader(port, 500);
//
//	char c;
//
//	if (!reader.read_char(c))
//		return false;
//
// Kevin Godden, www.ridgesolutions.ie
//

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/bind.hpp>
class blocking_reader {
  std::shared_ptr<boost::asio::serial_port> port;
  size_t timeout;
  boost::asio::deadline_timer timer;
  boost::asio::io_service &io;
  bool read_error;

  // Called when an async read completes or has been cancelled
  void read_complete(const boost::system::error_code &error,
                     size_t bytes_transferred);

  // Called when the timer's deadline expires.
  void time_out(const boost::system::error_code &error);

public:
  // Constructs a blocking reader, pass in an open serial_port and
  // a timeout in milliseconds.
  blocking_reader(std::shared_ptr<boost::asio::serial_port> port,
                  size_t timeout, boost::asio::io_service &io);
  // Reads a character or times out
  // returns false if the read times out
  bool read(std::vector<unsigned char> &buffer);
};

#endif
