#pragma once
#include <string>
#include <unordered_map>
#include <list>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>
#include <mysql.h>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <string>
#include <fstream>
#include <thread>

using SystemConfig = struct ST_SYSTEM_CONFIG;
extern boost::shared_mutex logMutex;


struct ST_SYSTEM_CONFIG
{
	std::list<std::string> GetConfigList()
	{
		std::list<std::string> configList;
		configList.push_back(HTTP_PORT);
		configList.push_back(HTTP_TOPIC);
		configList.push_back(ZEROMQ_IPC);
		configList.push_back(ZEROMQ_TOPIC);
		return configList;
	}

	std::string Get_HTTP_PORT()
	{
		return HTTP_PORT;
	}

	std::string Get_HTTP_TOPIC()
	{
		return HTTP_TOPIC;
	}

	std::string Get_ZEROMQ_IPC()
	{
		return ZEROMQ_IPC;
	}

	std::string Get_ZEROMQ_TOPIC()
	{
		return ZEROMQ_TOPIC;
	}

private:
	std::string HTTP_PORT = "HTTP.port";
	std::string HTTP_TOPIC = "HTTP.topic";
	std::string ZEROMQ_IPC = "ZeroMQ.ipc";
	std::string ZEROMQ_TOPIC = "ZeroMQ.topic";

};

void InitConfigMap(std::unordered_map<std::string, std::string>& configMap, SystemConfig& configNames);
void ReadSystemConfig(std::unordered_map<std::string, std::string>& configMap, SystemConfig& configNames);
std::thread StartMySqlTask();