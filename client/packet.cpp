#include "common.hpp"
#include "packet.hpp"
#include "utilities.hpp"

CPacket::CPacket() noexcept
    : m_header{MSG_VERSION, MSG_DEFAULT, 0},
      m_message{}
{
}

CPacket::CPacket(const unsigned int version,
               const unsigned int type,
               const unsigned int length,
               const std::string message) noexcept
    : m_header{version, type, length},
      m_message{message}
{
}

unsigned int CPacket::get_version() const noexcept { return m_header.m_version; }
unsigned int CPacket::get_message_type() const noexcept { return m_header.m_type; }
unsigned int CPacket::get_length() const noexcept { return m_header.m_length; }
unsigned int CPacket::get_header_size() const noexcept { return sizeof(m_header); }
unsigned int CPacket::get_message_size() const noexcept { return m_message.length(); }
unsigned int CPacket::get_packet_size() const noexcept { return get_header_size() + get_message_size(); }
std::string CPacket::get_message() const noexcept { return m_message; }

void CPacket::set_version(const unsigned int version) noexcept { m_header.m_version = version; }
void CPacket::set_type(const unsigned int type) noexcept { m_header.m_type = type; }
void CPacket::set_length(const unsigned int length) noexcept { m_header.m_length = length; }
void CPacket::set_message(const std::string message) noexcept { m_message = message; }

void CPacket::set(const unsigned int version,
                 const unsigned int type,
                 const unsigned int length) noexcept
{
    set_version(version);
    set_type(type);
    set_length(length);
}

void CPacket::set(const unsigned int version,
                 const unsigned int type,
                 const unsigned int length,
                 const std::string message) noexcept
{
    set(version, type, length);
    set_message(message);
}

boost::system::error_code CPacket::receive(boost::asio::ip::tcp::socket &socket) noexcept
{
    boost::system::error_code ec;
    read(socket, boost::asio::buffer(&m_header, get_header_size()), boost::asio::transfer_exactly(get_header_size()), ec);
    if (ec)
    {
        lock_stream();
        LOG_INF() << "Header read error! " << ec.message() << std::endl;
        unlock_stream();

        return ec;
    }

    const auto bytes_to_read = get_length() - get_header_size();
    if (bytes_to_read > 0)
    {
        boost::asio::streambuf read_buffer;
        read(socket, read_buffer, boost::asio::transfer_exactly(bytes_to_read), ec);
        if (ec)
        {
            LOG_ERR() << "Message read error! " << ec.message() << std::endl;
            return ec;
        }

        const std::string data_received((std::istreambuf_iterator<char>(&read_buffer)),
                                        std::istreambuf_iterator<char>());
        set_message(data_received);

        lock_stream();
        LOG_INF() << "Received: " << *this << std::endl;
        unlock_stream();
    }

    return ec;
}

boost::system::error_code CPacket::send(boost::asio::ip::tcp::socket &socket) noexcept
{
    lock_stream();
    LOG_INF() << "Sending packet: " << *this << std::endl;
    unlock_stream();

    // Send packet header
    boost::system::error_code ec;
    write(socket, boost::asio::buffer(&m_header, get_header_size()), boost::asio::transfer_exactly(get_header_size()), ec);
    if (ec)
    {
        lock_stream();
        LOG_ERR() << "Could not send header! ["
                  << get_peer_ip(socket) << ":"
                  << get_peer_port(socket)
                  << "]" << std::endl;
        unlock_stream();

        return ec;
    }

    // Send packet message
    write(socket, boost::asio::buffer(get_message(), get_message_size()), boost::asio::transfer_exactly(get_message_size()), ec);
    if (ec)
    {
        lock_stream();
        LOG_ERR() << "Could not send message! ["
                  << get_peer_ip(socket) << ":"
                  << get_peer_port(socket)
                  << "]" << std::endl;
        unlock_stream();

        return ec;
    }

    lock_stream();
    LOG_INF() << "Packet sent! ["
              << get_peer_ip(socket) << ":"
              << get_peer_port(socket)
              << "]..." << std::endl;
    unlock_stream();

    // Reset packet message
    set_message("");

    return ec;
}

boost::system::error_code CPacket::process(boost::asio::ip::tcp::socket &socket) noexcept
{
    // Receive welcome message from server
    auto ec = receive(socket);
    if (ec)
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    LOG_INF() << "Received: " << get_message() << std::endl;

    // Send command packet
    set_message("dir");
    set(MSG_VERSION, MSG_COMMAND, get_packet_size());

    ec = send(socket);
    if (ec)
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    // Receive command response packet
    ec = receive(socket);
    if (ec)
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    LOG_INF() << "Received: " << get_message() << std::endl;

    // Acknowledge command response packet
    set_message("");
    set(MSG_VERSION, MSG_DATA_ACK, get_packet_size());

    ec = send(socket);
    if (ec)
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    // Send EXIT command packet
    set_message("EXIT");
    set(MSG_VERSION, MSG_EXIT, get_packet_size());

    ec = send(socket);
    if (ec)
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    LOG_INF() << "Client session completed successfully!" << std::endl;

    return ec;
}

std::ostream &operator<<(std::ostream &os, const CPacket &packet)
{
    os << "Packet: {VERSION:"
       << std::hex << packet.get_version() << ", TYPE:0x"
       << std::hex << packet.get_message_type() << ", SIZE:"
       << std::dec << packet.get_packet_size() << "} | ";

    os << "Message: {SIZE:" << packet.get_message_size() << "} "
       << packet.get_message();

    return os;
}

bool CPacket::is_valid_version() const noexcept
{
    return (get_version() == MSG_VERSION);
}

bool CPacket::is_valid_msg_type() const noexcept
{
    return (get_message_type() == MSG_COMMAND ||
            get_message_type() == MSG_DATA_ACK ||
            get_message_type() == MSG_FIN_DATA_ACK ||
            get_message_type() == MSG_EXIT);
}
