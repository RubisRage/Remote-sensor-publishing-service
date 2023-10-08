#pragma once

#include <array>

#include <boost/asio.hpp>

class serialPortHandler {
public:
  static constexpr size_t buffer_size = 100;
  typedef std::array<char, buffer_size> buffer_type;

  serialPortHandler(const std::string &portName);
  ~serialPortHandler();
  std::array<char, buffer_size> read();

private:
  buffer_type data{0};
  boost::asio::io_context io;
  boost::asio::serial_port port{io};
};
