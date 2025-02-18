#include "ConfigMgr.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

std::string ConfigMgr::GetValue(const std::string& section_name, const std::string& key)
{
	//如果该节点下没有这个key
	if (config_map_[section_name].find(key) == config_map_[section_name].end()) { 
		return std::string();
	}

	return config_map_[section_name][key];
}

ConfigMgr::ConfigMgr()
{
	//获取当前工作目录，即生成的exe文件所在的目录
	boost::filesystem::path current_path = boost::filesystem::current_path();
	//配置文件的绝对路径
	boost::filesystem::path config_path = current_path / "../../../../config.ini";
	std::cout << "Config path:" << config_path << '\n';

	//通过property_tree读取，解析配置文件
	boost::property_tree::ptree pt;
	try {
		boost::property_tree::read_ini(config_path.string(), pt);
	} catch (const std::exception& e) {		/*善用trycatch可以查明异常发生的原因*/
		std::cout << e.what()<<'\n';   
		return;
	}

	//遍历所有section
	for (const auto& section : pt) {
		const std::string& section_name = section.first;  //节点名
		const boost::property_tree::ptree& section_tree = section.second; //一个节点下的全部键值对
		
		config_map_.insert(std::make_pair(section_name, std::unordered_map<std::string,std::string>()) );

		for (const auto& key_value_pair : section_tree) { //遍历一个节点下的键值对
			const std::string& key= key_value_pair.first;
			const std::string& value = key_value_pair.second.get_value<std::string>();
			//保存对应节点下的键值对
			config_map_[section_name].insert(std::pair<std::string,std::string>{ key,value });
		}
	}
	
	//输出全部的section和对应的键值对
	for (const auto& section : config_map_) {
		std::cout << '[' << section.first << "]\n"; //输出节点名
		for (const auto& key_value_pair : section.second) {
			std::cout << '\t'<<key_value_pair.first << " = " << key_value_pair.second << '\n';
		}
	}

}

ConfigMgr& ConfigMgr::GetInstance()
{
	static ConfigMgr cm;
	return cm;
}
