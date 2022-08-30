#pragma once

#include <boost/asio.hpp>

#include "common.hpp"
#include "packet.hpp"

class CClient final
{
public:
    CClient(const std::string ip, const unsigned short port) noexcept;
    ~CClient() noexcept;

    

    template<class T>
    void TransferDataToServer(T data) noexcept
    {
        ConnectViaSocket();
        m_packet.TransferData<T>(m_socket, data);
        DisconnectFromSocket();
    }

private:
    bool ConnectViaSocket() noexcept;
    void DisconnectFromSocket() noexcept;

    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::endpoint m_endpoint;
    boost::asio::ip::tcp::socket m_socket;
    CPacket m_packet;
};
