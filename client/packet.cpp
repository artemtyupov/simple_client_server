#include "common.hpp"
#include "packet.hpp"
#include "utilities.hpp"

CPacket::CPacket() noexcept
    : m_header{0, 0}
{
}

CPacket::CPacket(const unsigned int type,
               const unsigned int length) noexcept
    : m_header{type, length}
{
}

boost::system::error_code CPacket::TransferPacketHeader(boost::asio::ip::tcp::socket &socket) noexcept
{
    // Send packet header

    lock_stream();
    LOG_INF() << "Sending packet header: " << *this << std::endl;
    unlock_stream();

    boost::system::error_code ec;
    write(socket, boost::asio::buffer(&m_header, sizeof(m_header)), boost::asio::transfer_exactly(sizeof(m_header)), ec);
    if (ec)
    {
        lock_stream();
        LOG_ERR() << "Could not send header! ["
                  << GetPeerIP(socket) << ":"
                  << GetPeerPort(socket)
                  << "]" << std::endl;
        unlock_stream();
    }

    return ec;
}

std::ostream &operator<<(std::ostream &os, const CPacket &packet)
{
    os << "Packet: {TYPE:"
       << packet.GetPacketType() << ", SIZE:"
       << packet.GetPacketSize() << "} | ";

    return os;
}

