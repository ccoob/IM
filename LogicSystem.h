#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include <boost/json.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <map>
#include "const.h"
#include "singleton.h"

namespace json = boost::json;

class HttpConnection;
using HttpHandler = std::function<void(std::shared_ptr<HttpConnection>)>;

//定义请求路径path和相对应的回调，并通过公有接口HandleGet和HandlePost调用回调。
class LogicSystem : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem() = default; // 空实现
    bool HandleGet(std::string, std::shared_ptr<HttpConnection>);   //调用Get请求路径对应的回调（如果存在）
    void RegGet(std::string path, HttpHandler handler); // 注册get请求的路径和与之对应的回调
    bool HandlePost(std::string, std::shared_ptr<HttpConnection>);   //调用Post请求路径对应的回调（如果存在）
    void RegPost(std::string path, HttpHandler handler); // 注册post请求的路径和与之对应的回调
private:
    LogicSystem();  //定义path对应的回调函数

    std::map<std::string, HttpHandler> get_handlers_;   // 保存path对应的回调函数
    std::map<std::string, HttpHandler> post_handlers_; 
};

#endif