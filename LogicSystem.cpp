#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifygRPCClient.h"

void LogicSystem::RegGet(std::string path,HttpHandler handler)
{
    get_handlers_.insert(make_pair(path,handler));
}

void LogicSystem::RegPost(std::string path, HttpHandler handler)
{
    post_handlers_.insert(make_pair(path,handler)) ;
}

LogicSystem::LogicSystem()
{ 
    //测试Get方法的回调
    RegGet("/get_test",[](std::shared_ptr<HttpConnection> connection){
        //往应答中写入内容
        beast::ostream(connection->response_.body())<<"receive get_test reg \r\n";
        int i=0;
        //将url参数写回应答包体
        for(auto& args:connection->parameters_) {
            ++i;
            beast::ostream(connection->response_.body())
                <<"parameter "<<i<<" key is "<<args.first<<", value is "<<args.second<<"\r\n";
        }
    });

    //获取验证码的回调
    RegPost("/get_verifycode",[](std::shared_ptr<HttpConnection> connection){
        //将收到的请求，包体内容转换成字符串
        auto body_str = beast::buffers_to_string(connection->request_.body().data()); 
        std::cout<<"receive body is "<<body_str<<'\n';

        //设置应答的内容类型为json文本
        connection->response_.set(http::field::content_type,"text/json");

        //将json格式的字符串(body的内容)反序列化为json对象，解析失败抛出异常
        json::object src_root{json::parse(body_str).as_object()};

        //如果收到的内容能正确解析成json对象，且其中包含键:email
        if(src_root.if_contains("email")){ 
            std::string email = json::serialize(src_root["email"]) ; //将json值序列化为字符串
            std::cout<<"email is " <<email<<'\n';
            /****************调用gRPC服务************/
            GetVerifyRsp rsp = VerifygRPCClient::GetInstance()->GetVerifyCode(email);
            src_root["error"] = rsp.error();   //将gRPC请求结果保存
            //将json对象序列化成json格式的字符串，写入应答的包体中
            beast::ostream(connection->response_.body())<<json::serialize(src_root); 
            return true;
        }else{
            std::cout<<"email doesn't exist\n";
            src_root["error"] = ErrorCodes::Error_Json;
            beast::ostream(connection->response_.body())<<json::serialize(src_root); 
            return false;
        }
    });
}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> con)
{
    //path是请求的路径，即url中域名后面跟着的路径
    if(get_handlers_.find(path)==get_handlers_.end()) {
        //如果请求的路径不存在对应的回调，返回错误
        return false;
    }
    //调用之前注册的回调
    get_handlers_[path](con);
    return true;
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> con)
{
    //path是请求的路径，即url中域名后面跟着的路径
    if(post_handlers_.find(path)==post_handlers_.end()) {
        //如果请求的路径不存在对应的回调，返回错误
        return false;
    }
    //调用对象
    post_handlers_[path](con);
    return true;
}