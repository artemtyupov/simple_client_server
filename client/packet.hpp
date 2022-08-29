#pragma once

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "utilities.hpp"

class CPacket final
{
public:
    CPacket() noexcept;
    CPacket(const unsigned int type,
           const unsigned int length) noexcept;

    unsigned int GetPacketType() const noexcept { return m_header.m_type; }
    unsigned int GetPacketSize() const noexcept { return m_header.m_length; }

    void SetPacketType(const unsigned int type) noexcept { m_header.m_type = type; }
    void SetPacketSize(const unsigned int length) noexcept { m_header.m_length = length; }

    friend std::ostream &operator<<(std::ostream &os, const CPacket &packet);

    template<class T>
    void TransferData(boost::asio::ip::tcp::socket &socket,T data) noexcept
    {
        boost::system::error_code ec;
        if (std::is_pod<T>::value)
            ec = TransferPlainData<T>(socket, data);
        else
            ec = TransferStrongData<T>(socket, data);

        if (ec)
        {
            LOG_ERR() << "Error: " << ec.message() << std::endl;
            return;
        }

        lock_stream();
        LOG_INF() << "Packet has been sent." << std::endl;
        unlock_stream();
    }

private:

    boost::system::error_code TransferPacketHeader(boost::asio::ip::tcp::socket &socket) noexcept;

    template<class T>
    boost::system::error_code TransferPlainData(boost::asio::ip::tcp::socket &socket, T data) noexcept
    {
        boost::system::error_code ec;
        if (std::is_same<T, int>::value)
        {
            SetPacketType(ePacketType::ePacketType_int);
            SetPacketSize(sizeof(int));
            ec = TransferPacketHeader(socket);
        }
        else if (std::is_same<T, double>::value)
        {
            SetPacketType(ePacketType::ePacketType_double);
            SetPacketSize(sizeof(double));
            ec = TransferPacketHeader(socket);
        }
        
        // Send packet
        if (ec)
            write(socket, boost::asio::buffer(&data, GetPacketSize()), boost::asio::transfer_exactly(GetPacketSize()), ec);
            
        return ec;
    }

    template<class T>
    boost::system::error_code TransferStrongData(boost::asio::ip::tcp::socket &socket,T data) noexcept
    {
        boost::system::error_code ec;
        return ec;
    }

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
