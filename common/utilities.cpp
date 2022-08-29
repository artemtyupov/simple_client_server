#include "utilities.hpp"

std::string GetPeerIP(const boost::asio::ip::tcp::socket &socket) noexcept
{
    return socket.remote_endpoint().address().to_string();
}

unsigned short GetPeerPort(const boost::asio::ip::tcp::socket &socket) noexcept
{
    return socket.remote_endpoint().port();
}
