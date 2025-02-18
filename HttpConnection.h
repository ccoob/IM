#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/url.hpp>
#include <unordered_map>
#include "const.h"
#include "singleton.h"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace url = boost::urls;
namespace beast = boost::beast;
namespace http= boost::beast::http;


/*http连接管理类,用于管理一个连接 通信的过程*/
class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
    friend class LogicSystem;
public:
    HttpConnection(asio::io_context& ioc);
    void Start();           //监听读事件
    tcp::socket& GetSocket() { return socket_; }
private:
    void CheckDeadline();   //连接超时检测
    void WriteResponse();   //收到数据，给对方应答
    void HandleReq();       //处理请求
    tcp::socket socket_;    //当前连接 使用的套接字
    boost::beast::flat_buffer buffer_{8*1024};
    beast::http::request<http::dynamic_body> request_;     //请求
    http::response<http::dynamic_body> response_;   //应答
    //当前连接的计时器，默认60秒
    asio::steady_timer deadline_{ socket_.get_executor(),std::chrono::seconds(60) };

    std::string path_;   
    std::unordered_map<std::string,std::string> parameters_;  //保存url的参数
};

#endif