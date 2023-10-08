#pragma once

#include <array>

#include <boost/asio.hpp>

class serialPortHandler {
    public:
        serialPortHandler(const std::string& portName);
        ~serialPortHandler();
        std::array<char, 100> read();
    private:    
        std::array<char, 100> data {0};
        boost::asio::io_context io;
        boost::asio::serial_port port {io};
};