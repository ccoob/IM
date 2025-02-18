#include <iostream>
#include "CServer.h"
#include "ConfigMgr.h"

int main(int argc, char** argv)
{
    auto& CfgMgr=ConfigMgr::GetInstance();
    std::string gate_port_str = CfgMgr["GateServer"]["Port"];
    uint16_t gate_port;
    if ((gate_port = atoi(gate_port_str.c_str()))== 0)
    {
        std::cerr << "gate port error\n";
        return 0;
    }
    try {
        //uint16_t port{gate_port};    //服务器监听端口

        /*io_context类为异步I/O对象提供核心I/O功能。 */
        /*与服务器的accptor绑定的ioc,专门用来处理新连接事件*/
        asio::io_context ioc{1};    

        //为进程的打断(ctrl+c)和中止(kill)信号，创建一个信号集。
        boost::asio::signal_set signals(ioc,SIGINT,SIGTERM);
        //异步等待信号出现，信号出现后执行回调pwort 
        signals.async_wait ([&ioc](const boost::system::error_code error,int signal_number){
            if(error){ //出现错误
                return;
            }
            ioc.stop();
        });
        //创建一个服务器实例，开始监听连接。
        std::make_shared<CServer>(ioc,gate_port)->Start();
        std::cout << "GateServer starts,listening to port:" << gate_port << '\n';
        ioc.run();  //循环执行任务队列中的任务（主线程负责接收连接任务）

    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
}
