#pragma once
#include <vector>
#include <boost/asio.hpp>
#include "singleton.h"
#include "const.h"

namespace asio = boost::asio;

//io_context池，由多个子线程执行各自的io_context.run()
class AsioIOServicePool :public Singleton<AsioIOServicePool>
{
	friend  Singleton<AsioIOServicePool>;
public:
	using IOService = asio::io_context;
	using Work = asio::io_context::work;	//work绑定io_context，保证io_context.run()在没有事件的情况下也会一直阻塞
	using WorkPtr = std::unique_ptr<Work>;
	~AsioIOServicePool();
	AsioIOServicePool(const AsioIOServicePool&) = delete;

	//从io_context池中取出一个io_context
	asio::io_context& GetIOService();

	//停止服务池，回收一些资源，唤醒阻塞的线程
	void Stop();

private:
	AsioIOServicePool(std::size_t pool_size = 2 /*std::thread::hardware_concurrency()*/);
	std::vector<IOService> ioServices_;		//保存io_context的容器
	std::vector<WorkPtr> works_;			//保存io_context对应的work的容器
	std::vector<std::thread> threads_;		//保存每个io_context所在的子线程的容器
	std::size_t nextIOService_;				//下一次要从ioVervices_中取出的io_context的索引
};