#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "session.hpp"

class CServer final
{
public:
    CServer(const std::string ip, const unsigned short port,
           const unsigned int num_threads) noexcept;
    ~CServer() noexcept;

    void start() noexcept;
    void stop() noexcept;

private:
    void worker_thread_callback(boost::shared_ptr<boost::asio::io_service> ios) noexcept;
    void accept_handler(boost::shared_ptr<CSession> this_session, const boost::system::error_code &ec) noexcept;
    void accept_new_connection() noexcept;

    boost::shared_ptr<boost::asio::io_service> m_ios_acceptors;
    boost::shared_ptr<boost::asio::io_service::work> m_ios_work_acceptors;
    boost::shared_ptr<boost::asio::io_service> m_ios_executors;
    boost::shared_ptr<boost::asio::io_service::work> m_ios_work_executors;
    boost::thread_group m_executors_thread_group;

    boost::asio::ip::tcp::endpoint m_endpoint;
    boost::asio::ip::tcp::acceptor m_acceptor;

    boost::shared_ptr<CSession> m_session;

    boost::asio::signal_set m_signals;
};
