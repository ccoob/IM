#include "HttpConnection.h"
#include "LogicSystem.h"

HttpConnection::HttpConnection(asio::io_context& ioc): socket_(ioc)
{

}

void HttpConnection::Start()
{
    auto self = shared_from_this();
    /*
    1、当socket上有数据时，异步地读取一个完整的消息到request，
    2、如果正在读取的消息的末尾有额外数据，则将它读到buffer，以供下次读取消息时使用 
    3、读取完数据或有错误发生后调用回调(lambda)
    4、async_read调用后立刻返回 
    */
    boost::beast::http::async_read(socket_,buffer_,request_,
        [self](beast::error_code ec, std::size_t bytes_transferred){
            try {
                if(ec){   //读取过程中发生错误
                    std::cout<<"http read error is " <<ec.what()<<'\n';
                    return ;
                }
                boost::ignore_unused(bytes_transferred); //HTTP不需要做粘包处理
                self->HandleReq();    //处理请求
                self->CheckDeadline();
            } catch(const std::exception& e) {
                std::cerr <<"exception is "<< e.what() << '\n';
            }
    });

}

void HttpConnection::HandleReq()
{
    //根据请求的HTTP版本，设置应答的HTTP版本
    response_.version(request_.version());
    response_.keep_alive(false);    //不需要保活

    //构建完整的URL
    std::string url=std::string("http://localhost:6060")+std::string(request_.target()); 
    //解析URI，并将结果保存
    url::url_view uri=url::parse_uri(url).value();
    path_=uri.path();                   //保存path
    for(auto& args:uri.params()) {     //保存请求的参数
        //parameters_[args.key]=args.value; 
        parameters_.insert({ args.key,args.value });
    }

    bool success{false};
    if(request_.method()==http::verb::get){     
        //逻辑层单例类处理get请求
        success = LogicSystem::GetInstance()->HandleGet(path_,shared_from_this());
    }else if(request_.method()==http::verb::post){     
        //逻辑层单例类处理post请求
        success = LogicSystem::GetInstance()->HandlePost(path_,shared_from_this());
    }
    if(!success){       //如果解析请求失败
        //设置HTTP响应的状态码为404
        response_.result(http::status::not_found);
        //把应答的内容类型设置为纯文本
        response_.set(http::field::content_type,"text/plain");   
        //把应答包体的内容修改为指定文本
        beast::ostream(response_.body())<<"url not found\r\n";
    }else{
        response_.result(http::status::ok);
        response_.set(http::field::server,"GateServer"); //设置应答服务器名称
    }
    WriteResponse();
}

void HttpConnection::WriteResponse()
{
    auto self = shared_from_this();
    response_.content_length(response_.body().size()); //设置应答的内容长度

    /*将response的内容，异步地写入到socket。如果数据写完或是有错误发生，则调用回调。*/
    http::async_write(socket_,response_,[self](beast::error_code ec,std::size_t bytes_transferred){
        //如果发生错误（例如定时器到期socket被关闭）或是正常写完数据（在规定时间内写完）
        self->socket_.shutdown(tcp::socket::shutdown_send,ec);      //服务器不再发送数据。
        self->deadline_.cancel();           //因为服务器关闭连接，所以不需要这个连接的定时器了，取消它。   
    });
}

void HttpConnection::CheckDeadline()
{
    auto self = shared_from_this();
    deadline_.async_wait([self](beast::error_code ec){
        if(!ec) {/*如果倒计时结束前没出错
            则服务器将调用回调，主动关闭连接，如果此时客户端没关闭，则客户端会进入time_wait状态。*/
            self->socket_.close(ec);
        }
    });
}