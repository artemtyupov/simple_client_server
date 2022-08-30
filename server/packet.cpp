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

boost::system::error_code CPacket::GetDataFromSocket(boost::asio::ip::tcp::socket &socket) noexcept
{
    boost::system::error_code ec;
    read(socket, boost::asio::buffer(&m_header, sizeof(m_header)), boost::asio::transfer_exactly(sizeof(m_header)), ec);
    if (ec)
    {
        lock_stream();
        LOG_INF() << "Header read error! " << ec.message() << std::endl;
        unlock_stream();

        return ec;
    }
    
    lock_stream();
    LOG_INF() << "Received packet header: " << *this << std::endl;
    unlock_stream();

    const unsigned int bytes_to_read = GetPacketSize();
    if (bytes_to_read > 0)
    {
        boost::asio::streambuf read_buffer;
        read(socket, read_buffer, boost::asio::transfer_exactly(bytes_to_read), ec);
        if (ec)
        {
            LOG_ERR() << "Packet read error! " << ec.message() << std::endl;
            return ec;
        }

        if (m_header.m_type == ePacketType::ePacketType_int)
        {
            int data_reveived = *(int*)read_buffer.data().data();
            lock_stream();
            LOG_INF() << "Received value: " << data_reveived << std::endl;
            unlock_stream();
        }
        else if (m_header.m_type == ePacketType::ePacketType_double)
        {
            double data_reveived = *(double*)read_buffer.data().data();
            lock_stream();
            LOG_INF() << "Received value: " << data_reveived << std::endl;
            unlock_stream();
        }
        else
        {
            const std::string data_received((std::istreambuf_iterator<char>(&read_buffer)),
                                        std::istreambuf_iterator<char>());
        }
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