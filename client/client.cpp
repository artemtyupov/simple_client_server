#include "client.hpp"
#include "packet.hpp"

CClient::CClient(const std::string ip, const unsigned short port) noexcept
    : m_endpoint{boost::asio::ip::address::from_string(ip), port},
      m_socket{m_io_service}
{
    LOG_INF() << "Initiating client... [" << m_endpoint << "]" << std::endl;
}

CClient::~CClient() noexcept
{
    DisconnectFromSocket();
    LOG_INF() << "Client exited successfully!" << std::endl;
}

bool CClient::ConnectViaSocket() noexcept
{
    bool is_connected = true;

    LOG_INF() << "Connecting to server..." << std::endl;

    try
    {
        boost::system::error_code ec;
        m_socket.connect(m_endpoint, ec);
        if (ec)
        {
            LOG_ERR() << "Error: " << ec.message() << std::endl;
            is_connected = false;
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERR() << "Exception: " << e.what() << std::endl;
        is_connected = false;
    }

    if (is_connected)
    {
        LOG_INF() << "Connected successfully!" << std::endl;
    }
    else
    {
        LOG_ERR() << "Failed to connect to the server!" << std::endl;
    }

    return is_connected;
}

void CClient::DisconnectFromSocket() noexcept
{
    if (m_socket.is_open())
    {
        m_socket.close();
    }
}
