#pragma once

#include <string>
#include <boost/asio.hpp>

///Client packet types
enum ePacketType
{
    ePacketType_int,
    ePacketType_double,
    ePacketType_string
};

std::string GetPeerIP(const boost::asio::ip::tcp::socket &socket) noexcept;
unsigned short GetPeerPort(const boost::asio::ip::tcp::socket &socket) noexcept;
