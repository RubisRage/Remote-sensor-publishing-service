#include "serial.hpp"

serialPortHandler::serialPortHandler(const std::string &portName) {
  port.open(portName);
  port.set_option(boost::asio::serial_port::baud_rate(9600));
}

serialPortHandler::buffer_type serialPortHandler::read() {
  boost::asio::read(port, boost::asio::buffer(data));
  return data;
}

serialPortHandler::~serialPortHandler() { port.close(); }
