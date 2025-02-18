#pragma once
//#include "const.h"

///*配置项*/
//struct SectionInfo
//{
//	SectionInfo() = default;
//	~SectionInfo() {
//		section_datas_.clear();
//	}
//	SectionInfo(const SectionInfo& src) {
//		
//	}
//	std::map<std::string, std::string> section_datas_;
//	std::string operator[](const std::string& key) {
//		if (section_datas_.find(key) == section_datas_.end()) {
//			return "";
//		}
//		return	section_datas_[key];
//	}
//};

#include <unordered_map>
#include <iostream>

class ConfigMgr
{
public:
	ConfigMgr(const ConfigMgr&) = delete;
	ConfigMgr(ConfigMgr&&) = delete;
	ConfigMgr& operator=(ConfigMgr&) = delete;
	ConfigMgr& operator=(ConfigMgr&&) = delete;
	~ConfigMgr() { config_map_.clear(); }
	std::unordered_map<std::string,std::string>
		operator[](const std::string& config_name) {
		if (config_map_.find(config_name) == config_map_.end()) {
			return std::unordered_map<std::string, std::string>();
		}
		return config_map_[config_name];
	}
	
private:
	//通过配置项名称，找到对应的服务器配置
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> config_map_;
public:
	std::string GetValue(const std::string& section_name, const std::string& key);
	ConfigMgr() ;
	static ConfigMgr& GetInstance();
};
