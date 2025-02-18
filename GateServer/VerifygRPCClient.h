#pragma once
#include <atomic>
#include <queue>
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "singleton.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyReq;	//gRPC自定义请求类
using message::GetVerifyRsp;   //gRPC自定义回复类
using message::VerifyService;  //gRPC服务类

/*RPC连接池*/
class RPConPool
{
public:
	RPConPool(size_t pool_size, std::string host, std::string port);
		
	//从连接池中取出一个连接
	std::unique_ptr<VerifyService::Stub> getConnection();

	//用完连接后要放回去
	void returnConnection(std::unique_ptr<VerifyService::Stub> context);

	~RPConPool();

	void Close();
private:
	std::atomic<bool> b_stop_;	//标记连接池是否被析构
	size_t pool_size_;
	std::string host_;
	std::string port_;
	std::queue<std::unique_ptr<VerifyService::Stub>> connections_;	//连接队列
	std::condition_variable cond_;
	std::mutex mutex_;
};

//与gRPC服务器通信的类
class VerifygRPCClient:public Singleton<VerifygRPCClient>
{
	friend class Singleton<VerifygRPCClient>;		//允许单例类访问派生类的非公开成员
public:
	/*********外部调用gRPC服务的接口 *******/
	GetVerifyRsp GetVerifyCode(std::string email);
private:
	VerifygRPCClient();
	std::unique_ptr<RPConPool> pool_;
};

