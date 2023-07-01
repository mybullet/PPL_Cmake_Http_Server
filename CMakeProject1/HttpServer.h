#pragma once

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <list>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

using namespace std;
using namespace boost::asio;

extern std::string timeStr;
extern boost::shared_mutex timeStrMutex;
extern boost::shared_mutex logMutex;

class HttpServer
{
public:
    HttpServer(string realStr, int port = 80, bool run = true);

    void StartWebService();

    void CloseWebService();

    void handleRequest(ip::tcp::socket& socket);

private:
    void readDistDirecrotyInfo(std::list<std::string>& fileInfoList);
    std::string readFile(const std::string& filename);
    void processFilename(std::string& filename, const std::list<std::string>& fileInfoList);
    std::string getRegularHTTPResponseHeader(const std::string& filename, const std::string& fileContent);
    std::string getHTTP404NotFoundResponse();
    std::string extractRequestedFilename(std::string requestStr);
    std::string getRealtimeHTTPResponseHeader(std::string& timeStr);
    std::string getTimeStr(std::string& str = timeStr, boost::shared_mutex& strMutex = timeStrMutex);
    
private:
    std::string RealtimeDataUrl;
    std::list<std::string> FileInfoList;


    int m_port;
    bool m_run;
};
