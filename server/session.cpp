#include <exception>

#include "common.hpp"
#include "session.hpp"
#include "packet.hpp"
#include "utilities.hpp"

CSession::CSession(boost::shared_ptr<boost::asio::io_service> ios) noexcept
    : m_io_service{ios},
      m_socket{*m_io_service},
      m_packet{}
{
}

boost::asio::ip::tcp::socket &CSession::GetSocket() noexcept
{
    return m_socket;
}

void CSession::Start() noexcept
{
    try
    {
        m_packet.GetDataFromSocket(m_socket);
        Stop();
    }
    catch (const std::exception &e)
    {
        lock_stream();
        LOG_ERR() << "Exception: " << e.what() << std::endl;
        unlock_stream();
    }
}

void CSession::Stop() noexcept
{
    m_socket.shutdown(boost::asio::socket_base::shutdown_both);
    m_socket.close();
}
