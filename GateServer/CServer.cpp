#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"

//endpoint使用本地IP地址和端口号进行构造一个socket
CServer::CServer(boost::asio::io_context& ioc,unsigned short& port)
    :ioc_(ioc), acceptor_(ioc,tcp::endpoint(tcp::v4(),port))
{

}

void CServer::Start()
{
    auto self=shared_from_this();   //引用计数+1，防止当前Server对象被析构

    //从池中取出一个io_context，用来给新连接提供异步读写支持
    auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
    std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);
    
    //async_accept调用后立刻返回，接受请求后调用注册的回调函数，新的连接将写入到第一个参数
    //如果过程没出错，ec为false，反之则为true
    acceptor_.async_accept(new_con->GetSocket(), [self, new_con](beast::error_code ec) {
        try {
            if(ec){     
                self->Start();   //如果当前连接出错，则重新监听新的连接
                return ;
            }
            new_con->Start();       //通过新的httpConnection监听读写事件

            self->Start();      //继续监听连接事件
        } catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    });
};
