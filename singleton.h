#ifndef SINGLETON_H
#define SINGLETON_H
#include <iostream>
#include <memory>
#include <mutex>
/*单例类模板，用来管理某种类型的唯一实例，该类必须依靠子类类型特化，从而为子类生成代码*/
template<typename T>
class Singleton
{
protected:		//构造函数设置成protect，能让类外无法实例化的同时，子类可以继承该类并调用基类构造
    Singleton()=default;
    Singleton(const Singleton&)=delete;      //该类无法自己实例化，所以不需要构造和赋值
    Singleton& operator=(const Singleton&)=delete;
    static std::shared_ptr<T> instance_;	//声明静态share_ptr成员，用来管理对象实例
public:
    static std::shared_ptr<T> GetInstance()
    {
        static std::once_flag s_flag;	//once_flag配合call_once保证可调用对象f(此处为lambda)被多个线程并发调用时，仅仅只被调用一次
        std::call_once(s_flag,[&](){//不用make_share是因为T类型要继承Singleton，而它的构造是受保护的，make_share无法访问受保护的构造函数。
            instance_=std::shared_ptr<T>(new T) ;
        });
        return instance_;
    }
    void PrintAddress()//打印单例对象的地址。
    {
        std::cout<<instance_.get()<<'\n';
    }
    ~Singleton(){std::cout<<"Singleton destruct\n";}
};

template<typename T>
std::shared_ptr<T> Singleton<T>::instance_=nullptr; 	//定义并初始化智能指针

#endif // SINGLETON_H
