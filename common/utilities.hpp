#pragma once

#include <string>
#include <boost/asio.hpp>

std::string get_peer_ip(const boost::asio::ip::tcp::socket &socket) noexcept;
unsigned short get_peer_port(const boost::asio::ip::tcp::socket &socket) noexcept;
