#include "HttpServer.h"



void HttpServer::readDistDirecrotyInfo(std::list<std::string>& fileInfoList)
{
    // Initialize the list
    fileInfoList.clear();

    // Get the current directory
    boost::filesystem::path currentDir = boost::filesystem::current_path();

    // Get the path to the dist directory
    boost::filesystem::path distDir = currentDir / "dist";

    // Iterate through the files and directories in the dist directory
    for (boost::filesystem::recursive_directory_iterator it(distDir); it != boost::filesystem::recursive_directory_iterator(); ++it)
    {
        // Add the relative path to the list if it is a file
        if (boost::filesystem::is_regular_file(it->path()))
        {
            std::string path = "./" + boost::filesystem::relative(it->path(), currentDir).string();
            std::replace(path.begin(), path.end(), '\\', '/');
            fileInfoList.push_back(path);
        }
    }
}

std::string HttpServer::readFile(const std::string& filename)
{
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    return content;
}

void HttpServer::processFilename(std::string& filename, const std::list<std::string>& fileInfoList)
{
    if (filename.empty())
    {
        for (const auto& fileInfo : fileInfoList)
        {
            if (boost::algorithm::ends_with(fileInfo, ".html"))
            {
                filename = fileInfo;
                break;
            }
        }
    }
    else
    {
        for (const auto& fileInfo : fileInfoList)
        {
            if (fileInfo.find(filename) != std::string::npos)
            {
                filename = fileInfo;
                break;
            }
        }

        //如果是拉取log,则走这里
    }
}

std::string HttpServer::getRegularHTTPResponseHeader(const std::string& filename, const std::string& fileContent)
{
    std::string response_header = "HTTP/1.1 200 OK\r\n";
    if (filename.find(".js") != std::string::npos)
    {
        response_header += "Content-Type: application/javascript\r\n";
    }
    else if (filename.find(".css") != std::string::npos)
    {
        response_header += "Content-Type: text/css\r\n";
    }
    else if (filename.find(".svg") != std::string::npos)
    {
        response_header += "Content-Type: image/svg+xml\r\n";
    }
    else if (filename.find(".ico") != std::string::npos)
    {
        response_header += "Content-Type: image/x-icon\r\n";
    }
    else
    {
        response_header += "Content-Type: text/html\r\n";
    }
    response_header += "Content-Length: " + std::to_string(fileContent.size()) + "\r\n";
    response_header += "\r\n";
    return response_header;
}

std::string HttpServer::getHTTP404NotFoundResponse()
{
    // Send HTTP 404 Not Found response
    std::string response_header = "HTTP/1.1 404 Not Found\r\n";
    response_header += "Content-Type: text/html\r\n";
    response_header += "Content-Length: 21\r\n";
    response_header += "\r\n";
    response_header += "<h1>404 Not Found</h1>";
    return response_header;
}

std::string HttpServer::extractRequestedFilename(std::string requestStr)
{
    std::string filename;
    size_t pos1 = requestStr.find("GET /");
    size_t pos2 = requestStr.find(" HTTP/1.");
    if (pos1 != std::string::npos && pos2 != std::string::npos)
    {
        filename = requestStr.substr(pos1 + 5, pos2 - pos1 - 5);
        if (RealtimeDataUrl != filename)
        {
            processFilename(filename, FileInfoList);
        }
    }
    else
    {
        filename = "";
    }

    return filename;
}

std::string HttpServer::getTimeStr(std::string& str, boost::shared_mutex& strMutex)
{
    boost::shared_lock<boost::shared_mutex> lock(timeStrMutex);
    std::string temp = timeStr;
    lock.unlock();

    if(temp.empty())
    {
        temp = "the server is offline";
    }
    return temp;
};

std::string HttpServer::getRealtimeHTTPResponseHeader(std::string& timeStr)
{
    // Get the current system time
    /*time_t now = time(nullptr);
    timeStr = ctime(&now);*/
    timeStr = getTimeStr();
    
    // Send HTTP response header
    std::string response_header = "HTTP/1.1 200 OK\r\n";
    response_header += "Content-Type: text/plain\r\n";
    response_header += "Content-Length: " + std::to_string(timeStr.size()) + "\r\n";
    response_header += "\r\n";

    return response_header;
}

HttpServer::HttpServer(string realStr, int port, bool run):m_run(run), m_port(port)
{
    RealtimeDataUrl = realStr;
    readDistDirecrotyInfo(FileInfoList);
}

void HttpServer::StartWebService()
{
    try
    {
        boost::asio::io_context io_context;

        // Create an acceptor to listen on port 80
        boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), m_port));

        // Start accepting incoming connections
        while (m_run)
        {
            boost::asio::ip::tcp::socket socket(io_context);

            std::cout << "accepting" << std::endl;

            acceptor.accept(socket);

            std::cout << "accepted" << std::endl;

            // Create a new thread for each incoming connection
            std::thread t(&HttpServer::handleRequest, this, std::move(socket));
            t.detach();
        }
    }
    catch (const boost::system::system_error& ex)
    {
        std::cout << "Exception occurred: " << ex.what() << std::endl;
        CloseWebService();
    }  
}

void HttpServer::CloseWebService()
{
    m_run = false;
}

void HttpServer::handleRequest(ip::tcp::socket& socket)
{
    std::cout << "start thread : " << std::this_thread::get_id() << std::endl;

    boost::asio::streambuf request;
    boost::system::error_code ec;

    std::cout << "before read " << std::endl;
    boost::asio::read_until(socket, request, "\r\n\r\n", ec);

    std::cout << "after read " << std::endl;

    if (!ec)
    {
        std::string requestStr(boost::asio::buffers_begin(request.data()),
            boost::asio::buffers_end(request.data()));

        std::cout << "requestStr : " << requestStr << std::endl;

        std::string filename = extractRequestedFilename(requestStr);
        std::cout << "filename : " + filename << std::endl;

        if (filename == RealtimeDataUrl)
        {
            std::string timeStr;
            std::string response_header = getRealtimeHTTPResponseHeader(timeStr);

            std::cout << "send Real time : " << timeStr << std::endl;

            boost::asio::write(socket, boost::asio::buffer(response_header));
            boost::asio::write(socket, boost::asio::buffer(timeStr));
        }
        else
        {
            if (filename == "/test_log/this.log" || filename == "/test_log/all.log")
            {
                boost::unique_lock<boost::shared_mutex> lock(logMutex);
                if (boost::filesystem::exists(filename))
                {
                    std::string fileContent = readFile(filename);
                    std::cout << "fileContent : " << fileContent << std::endl;

                    std::string response_header = getRegularHTTPResponseHeader(filename, fileContent);
                    boost::asio::write(socket, boost::asio::buffer(response_header));
                    boost::asio::write(socket, boost::asio::buffer(fileContent));
                }
                else
                {
                    std::cout << "no exists filename" << std::endl;
                    std::string response_header = getHTTP404NotFoundResponse();
                    boost::asio::write(socket, boost::asio::buffer(response_header));
                }
                lock.unlock();
            }
            else
            {
                if (boost::filesystem::exists(filename))
                {
                    std::string fileContent = readFile(filename);
                    std::cout << "fileContent : " << fileContent << std::endl;

                    std::string response_header = getRegularHTTPResponseHeader(filename, fileContent);
                    boost::asio::write(socket, boost::asio::buffer(response_header));
                    boost::asio::write(socket, boost::asio::buffer(fileContent));
                }
                else
                {
                    std::cout << "no exists filename" << std::endl;
                    std::string response_header = getHTTP404NotFoundResponse();
                    boost::asio::write(socket, boost::asio::buffer(response_header));
                }
            }
            
        }
            
            
    }
    else
    {
        std::cerr << "Error reading request: " << ec.message() << std::endl;
    }

    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    std::cout << "close thread : " << std::this_thread::get_id() << std::endl;
}
