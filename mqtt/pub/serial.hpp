#pragma once

#include <array>

#include <boost/asio.hpp>
#include <cstdint>
#include <string>

class serialPortHandler {
public:
  static constexpr std::size_t buffer_size = 40;
  typedef std::array<uint8_t, buffer_size> buffer_type;

  serialPortHandler(const std::string &portName);
  ~serialPortHandler();
  buffer_type read();

private:
  buffer_type data{0};
  boost::asio::io_context io;
  boost::asio::serial_port port{io};
};
