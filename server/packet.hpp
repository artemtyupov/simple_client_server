#pragma once

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "message.hpp"

class CPacket final
{
public:
    CPacket() noexcept;
    CPacket(const unsigned int version,
           const unsigned int type,
           const unsigned int length,
           const std::string message) noexcept;

    unsigned int get_version() const noexcept;
    unsigned int get_message_type() const noexcept;
    unsigned int get_length() const noexcept;
    unsigned int get_header_size() const noexcept;
    unsigned int get_message_size() const noexcept;
    unsigned int get_packet_size() const noexcept;
    std::string get_message() const noexcept;

    void set_version(const unsigned int version) noexcept;
    void set_type(const unsigned int type) noexcept;
    void set_length(const unsigned int length) noexcept;
    void set_message(const std::string message) noexcept;

    void set(const unsigned int version,
             const unsigned int type,
             const unsigned int length) noexcept;
    void set(const unsigned int version,
             const unsigned int type,
             const unsigned int length,
             const std::string message) noexcept;

    boost::system::error_code receive(boost::asio::ip::tcp::socket &socket) noexcept;
    boost::system::error_code send(boost::asio::ip::tcp::socket &socket) noexcept;
    boost::system::error_code process(boost::asio::ip::tcp::socket &socket) noexcept;

    friend std::ostream &operator<<(std::ostream &os, const CPacket &packet);

private:
    boost::system::error_code welcome_client(boost::asio::ip::tcp::socket &socket) noexcept;
    boost::system::error_code process_client_command(const std::string cmd, boost::asio::ip::tcp::socket &socket) noexcept;
    boost::system::error_code acknowledge_client_response(boost::asio::ip::tcp::socket &socket) noexcept;

    bool is_valid_version() const noexcept;
    bool is_valid_msg_type() const noexcept;

    struct struct_header final
    {
        enum
        {
            VERSION_SIZE = 3,
            TYPE_SIZE = 5,
            LENGTH_SIZE = 16
        };

        unsigned int m_version : VERSION_SIZE;
        unsigned int m_type : TYPE_SIZE;
        unsigned int m_length : LENGTH_SIZE;
    };

    struct_header m_header;
    std::string m_message;
};
