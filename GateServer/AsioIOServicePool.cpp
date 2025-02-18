#include "AsioIOServicePool.h"

AsioIOServicePool::AsioIOServicePool(std::size_t size):ioServices_(size),
works_(size),nextIOService_(0)
{
	for (size_t i = 0; i < size; i++)
	{//创建work，并将io_context绑定到work上**************
		works_[i] = std::make_unique<Work>(ioServices_[i]);
	}

	for (size_t i = 0; i < ioServices_.size(); i++)
	{
		//创建子线程，并提交任务
		threads_.emplace_back([this, i] {
			ioServices_[i].run();
			});
	}

}

AsioIOServicePool::~AsioIOServicePool()
{
	Stop();//RAII，申请资源的对象自己释放资源
	std::cout << "AsioIOServicePool destruct\n";
}

asio::io_context& AsioIOServicePool::GetIOService()
{
	asio::io_context& service = ioServices_[nextIOService_++];
	nextIOService_ %= ioServices_.size();
	return service;
}

void AsioIOServicePool::Stop()
{
	for (auto& work:works_ )
	{
		work->get_io_context().stop();	//让io_context.run()立刻返回
		//reset调用unique_ptr的析构函数，其中会再调用Work的析构函数，这会回收work对应的io_context
		work.reset(); 
	}
	for (auto& th : threads_)
	{
		th.join();//等待所有子线程退出，如果不等待，可能子线程还没来得及退出池子就销毁了
	}
}
