/*全局公共头文件，声明一些全局类型，和简化别名*/
#pragma once
//namespace beast = boost::beast;         // from <boost/beast.hpp>
//namespace http = beast::http;           // from <boost/beast/http.hpp>
//namespace asio= boost::asio;            // from <boost/asio.hpp>
//using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//声明错误码（叫结果码更好）枚举类型
enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,  //Json解析错误
	RPCFailed = 1002,  //RPC请求错误
	VarifyExpired = 1003, //验证码过期
	VarifyCodeErr = 1004, //验证码错误
	UserExist = 1005,       //用户已经存在
	PasswdErr = 1006,    //密码错误
	EmailNotMatch = 1007,  //邮箱不匹配
	PasswdUpFailed = 1008,  //更新密码失败
	PasswdInvalid = 1009,   //密码更新失败
	TokenInvalid = 1010,   //Token失效
	UidInvalid = 1011,  //uid无效
};
