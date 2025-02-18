#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include "const.h"

namespace beast = boost::beast;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

class CServer:public std::enable_shared_from_this<CServer>
{
public:
    CServer(boost::asio::io_context& ioc,unsigned short& port);
    void Start();


private:
    boost::asio::ip::tcp::acceptor acceptor_;    //接收器
    boost::asio::io_context& ioc_;
};
