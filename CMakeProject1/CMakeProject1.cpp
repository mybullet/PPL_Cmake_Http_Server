// CMakeProject1.cpp: 定义应用程序的入口点。
//
#include "CMakeProject1.h"
#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include "common.h"
#include <unordered_map>
#include<mysql.h>

using namespace std;

std::string timeStr;
boost::shared_mutex timeStrMutex;
std::unordered_map<std::string, std::string> configMap;
SystemConfig configNames;

boost::shared_mutex logMutex;

int main()
{
    InitConfigMap(configMap, configNames);
    ReadSystemConfig(configMap, configNames);
    
    HttpServer webServer(configMap.find(configNames.Get_HTTP_TOPIC())->second, std::stoi(configMap.find(configNames.Get_HTTP_PORT())->second));
    std::thread webThread(&HttpServer::StartWebService, &webServer);
//
    ZmqSubscriber subscriber(configMap.find(configNames.Get_ZEROMQ_IPC())->second, configMap.find(configNames.Get_ZEROMQ_TOPIC())->second);
    std::thread subThread(&ZmqSubscriber::StartSubscriberService, &subscriber);

    //一分钟执行一次的子线程
    StartMySqlTask().join();

    webThread.join();
    subThread.join();
    return 0;
}

