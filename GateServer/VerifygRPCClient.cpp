#include "VerifygRPCClient.h"
#include "ConfigMgr.h"

GetVerifyRsp VerifygRPCClient::GetVerifyCode(std::string email)
{
	ClientContext context;  //客户端通信上下文
	GetVerifyRsp reply;		//应答类型
	GetVerifyReq request;	//请求类型
	request.set_email(email);

	auto stub = pool_->getConnection();
	//通过stub调用rpc服务的接口
	Status status= stub->GetVerifyCode(&context, request, &reply);
	if (status.ok()) {
		//do something
	}
	else {
		reply.set_error(ErrorCodes::RPCFailed);   //设置结果码
	}
	pool_->returnConnection(std::move(stub));
	return reply;
}

VerifygRPCClient::VerifygRPCClient() 
{
	auto& CfgMgr = ConfigMgr::GetInstance();
	std::string host = CfgMgr["VerifyServer"]["Host"];
	std::string port = CfgMgr["VerifyServer"]["Port"];
	pool_.reset(new RPConPool(5, host, port));
}

RPConPool::RPConPool(size_t pool_size, std::string host, std::string port) :
	pool_size_(pool_size), host_(host), port_(port), b_stop_(false) 
{
	for (size_t i = 0; i < pool_size; i++)
	{
		//1、创建与gRPC服务端通信的通道
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host+":"+port,
			grpc::InsecureChannelCredentials()); 
		//2、使用通道创建stub，并将其放入连接队列中
		connections_.push(VerifyService::NewStub(channel));
	}
}

std::unique_ptr<VerifyService::Stub> RPConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(mutex_);
	cond_.wait(lock, [this] {
		if (b_stop_)	//线程醒来并抢到锁后，先判断连接池是否被析构
		{
			return true; //返回true，代码将继续执行
		}
		//如果连接池没析构，但连接队列为空时，返回false，wait会解锁并重新进入阻塞
		return !connections_.empty();
		});
	if (b_stop_)	//连接池被析构，返回空连接
	{
		return nullptr;
	}
	auto context = std::move(connections_.front()); //将队头元素【移动】给context
	connections_.pop();		//删除队头元素
	return context;
}
 
void RPConPool::returnConnection(std::unique_ptr<VerifyService::Stub> context)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_)
	{//如果连接池被析构也别还了，直接返回
		return;
	}
	connections_.push(std::move(context));
	cond_.notify_one();	//通知一个正在等待连接队列的线程，去取连接
}

RPConPool::~RPConPool()
{
	std::lock_guard<std::mutex> lock(mutex_);
	Close();
	while (!connections_.empty())
	{
		connections_.pop();
	}
}

void RPConPool::Close()
{
	b_stop_ = true;
	cond_.notify_all();	//唤醒所有正在等待条件变量cond的线程
}
