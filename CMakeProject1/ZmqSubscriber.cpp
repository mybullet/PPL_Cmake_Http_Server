#include "ZmqSubscriber.h"
#include <filesystem>
#include <unistd.h>

ZmqSubscriber::ZmqSubscriber(std::string ipc, std::string topic, bool run):m_run(run),m_ipc(ipc),m_topic(topic)
{
    messageReceived.connect([&](std::string message) {
        //д��
        boost::unique_lock<boost::shared_mutex> lock(timeStrMutex);
        timeStr = message;
        std::cout << "Update Zmq Message: " << timeStr << std::endl;
        });
}
void ZmqSubscriber::CloseSubscriberService()
{
    m_run = false;
}
void ZmqSubscriber::StartSubscriberService()
{
    m_run = true;

    zmq::context_t context(1);
    zmq::socket_t subscriber(context, zmq::socket_type::sub);

    std::cout << "before connect" << m_ipc.c_str() << std::endl;


    // std::string filePath = "/tmp/test.ipc";

    // if (std::filesystem::exists(filePath) /*&& std::filesystem::is_regular_file(filePath)*/)
    // {
    //     std::cout << "File exists and is accessible" << std::endl;
    // }
    // else
    // {
    //     std::cout << "File does not exist or is not accessible" << std::endl;
    // }

    subscriber.connect(m_ipc.c_str());

    std::cout << "after connect" << std::endl;

    // Check if the connection is successful
    // int events = 0;
    // size_t eventsSize = sizeof(events);
    // subscriber.getsockopt(ZMQ_EVENTS, &events, &eventsSize);
    // if (events & ZMQ_POLLOUT) {
    //     std::cout << "ZMQ Connected successfully" << std::endl;
    // } else {
    //     int errorNumber = zmq_errno();
    //     std::string errorMessage = zmq_strerror(errorNumber);
    //     std::cout << "ZMQ Connection failed: " << errorMessage << std::endl;
    //     return;
    // }

    subscriber.setsockopt(ZMQ_SUBSCRIBE, m_topic.c_str(), 5);

    while (m_run) 
    {   
        zmq::message_t topic, message;

        std::cout << "before recv" << std::endl;
        subscriber.recv(topic, zmq::recv_flags::none);
        subscriber.recv(message, zmq::recv_flags::none);
        std::cout << "after recv" << std::endl;

        std::string topic_str(static_cast<char*>(topic.data()), topic.size());
        std::string message_str(static_cast<char*>(message.data()), message.size());
        std::cout << "Receive Topic: " << topic_str << ", Message: " << message_str << std::endl;

        messageReceived(message_str);

        boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    }
    subscriber.close();
    context.close();
}