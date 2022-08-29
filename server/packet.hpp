#pragma once

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

class CPacket final
{
public:
    CPacket() noexcept;
    CPacket(const unsigned int type,
           const unsigned int length) noexcept;

    boost::system::error_code GetDataFromSocket(boost::asio::ip::tcp::socket &socket) noexcept;

    unsigned int GetPacketType() const noexcept { return m_header.m_type; }
    unsigned int GetPacketSize() const noexcept { return m_header.m_length; }

    void SetPacketType(const unsigned int type) noexcept { m_header.m_type = type; }
    void SetPacketSize(const unsigned int length) noexcept { m_header.m_length = length; }

    friend std::ostream &operator<<(std::ostream &os, const CPacket &packet);

private:
    struct PacketHeader final
    {
        enum
        {
            TYPE_SIZE = 5,
            LENGTH_SIZE = 16
        };

        unsigned int m_type : TYPE_SIZE;
        unsigned int m_length : LENGTH_SIZE;
    };

    PacketHeader m_header;
};
