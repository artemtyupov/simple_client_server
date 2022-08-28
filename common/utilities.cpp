#include "utilities.hpp"

std::string get_peer_ip(const boost::asio::ip::tcp::socket &socket) noexcept
{
    return socket.remote_endpoint().address().to_string();
}

unsigned short get_peer_port(const boost::asio::ip::tcp::socket &socket) noexcept
{
    return socket.remote_endpoint().port();
}
