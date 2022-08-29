#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "packet.hpp"

class CSession final
{
public:
    CSession(boost::shared_ptr<boost::asio::io_service> ios) noexcept;

    void Start() noexcept;
    void Stop() noexcept;

    boost::asio::ip::tcp::socket& GetSocket() noexcept;

private:
    boost::shared_ptr<boost::asio::io_service> m_io_service;
    boost::asio::ip::tcp::socket m_socket;
    CPacket m_packet;
};
