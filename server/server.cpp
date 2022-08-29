#include <exception>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/system/error_code.hpp>

#include "common.hpp"
#include "server.hpp"
#include "utilities.hpp"

CServer::CServer(const std::string ip, const unsigned short port,
               const unsigned int num_threads) noexcept
    : m_ios_acceptors{boost::make_shared<boost::asio::io_service>()},
      m_ios_work_acceptors{boost::make_shared<boost::asio::io_service::work>(*m_ios_acceptors)},
      m_ios_executors{boost::make_shared<boost::asio::io_service>()},
      m_ios_work_executors{boost::make_shared<boost::asio::io_service::work>(*m_ios_executors)},
      m_endpoint{boost::asio::ip::address::from_string(ip), port},
      m_acceptor{*m_ios_acceptors, m_endpoint},
      m_session{boost::make_shared<CSession>(m_ios_executors)},
      m_signals{*m_ios_acceptors, SIGINT, SIGTERM}
{
    // Add signal handling for graceful termination (CTRL + C)
    m_signals.async_wait(boost::bind(&CServer::Stop, this));

    LOG_INF() << "Initiating server..." << std::endl;
    for (unsigned int i = 0; i < num_threads; ++i)
    {
        m_executors_thread_group.create_thread(boost::bind(&CServer::WorkerThreadCallback,
                                                           this,
                                                           m_ios_executors));
    }
    
    m_acceptor.async_accept(m_session->GetSocket(),
                            boost::bind(&CServer::AcceptHandler, this, m_session, boost::asio::placeholders::error));

    lock_stream();
    LOG_INF() << "Server started! [" << m_endpoint << "]" << std::endl;
    unlock_stream();
}

CServer::~CServer() noexcept
{
    Stop();

    lock_stream();
    LOG_INF() << "Server closed successfully! Bye bye! :)" << std::endl;
    unlock_stream();
}

void CServer::Start() noexcept
{
    m_ios_acceptors->run();
}

void CServer::Stop() noexcept
{
    if (!m_ios_acceptors->stopped())
    {
        m_ios_acceptors->stop();
    }

    if (!m_ios_executors->stopped())
    {
        m_ios_executors->stop();
    }
}

// Utility methods

void CServer::WorkerThreadCallback(boost::shared_ptr<boost::asio::io_service> ios) noexcept
{
    while (true)
    {
        try
        {
            boost::system::error_code ec;
            ios->run(ec);
            if (ec)
            {
                lock_stream();
                LOG_ERR() << " Error: " << ec.message() << std::endl;
                unlock_stream();
            }
        }
        catch (const std::exception &e)
        {
            lock_stream();
            LOG_ERR() << " Exception: " << e.what() << std::endl;
            unlock_stream();
        }
    }
}

void CServer::AcceptHandler(boost::shared_ptr<CSession> this_session, const boost::system::error_code &ec) noexcept
{
    if (!ec)
    {
        LOG_INF() << "Connection established with client! ["
                  << GetPeerIP(this_session->GetSocket()) << ":"
                  << GetPeerPort(this_session->GetSocket()) << "]" << std::endl;

        m_ios_executors->post(boost::bind(&CSession::Start, this_session));

        m_session = boost::make_shared<CSession>(m_ios_executors);
        m_acceptor.async_accept(m_session->GetSocket(),
                                boost::bind(&CServer::AcceptHandler, this, m_session, boost::asio::placeholders::error));
    }
}
