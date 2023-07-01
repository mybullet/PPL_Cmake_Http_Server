#include "ZmqSubscriber.h"

ZmqSubscriber::ZmqSubscriber(std::string ipc, std::string topic, bool run):m_run(run),m_ipc(ipc),m_topic(topic)
{
    messageReceived.connect([&](std::string message) {
        //写锁
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

    //subscriber.connect("ipc://Y:/qt_project/ZmqServer/build/tmp/test.ipc");
    subscriber.connect(m_ipc.c_str());
    subscriber.setsockopt(ZMQ_SUBSCRIBE, m_topic.c_str(), 5);

    while (m_run) 
    {   
        zmq::message_t topic, message;
        subscriber.recv(topic, zmq::recv_flags::none);
        subscriber.recv(message, zmq::recv_flags::none);

        std::string topic_str(static_cast<char*>(topic.data()), topic.size());
        std::string message_str(static_cast<char*>(message.data()), message.size());
        std::cout << "Receive Topic: " << topic_str << ", Message: " << message_str << std::endl;

        //发射信号，使用message_str更新timeStr
        messageReceived(message_str);

        boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    }
    subscriber.close();
    context.close();
}