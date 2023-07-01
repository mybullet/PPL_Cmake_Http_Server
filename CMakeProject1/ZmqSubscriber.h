#pragma once

#include <string>
#include <fstream>
#include "zmq.hpp"
#include <boost/signals2.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <iostream>
#include <boost/thread.hpp>

extern std::string timeStr;
extern boost::shared_mutex timeStrMutex;

class ZmqSubscriber
{
public:
	ZmqSubscriber(std::string ipc, std::string topic = "STATE", bool run = true);
	void StartSubscriberService();
	void CloseSubscriberService();
private:
	bool m_run;
	std::string m_ipc;
	std::string m_topic;
	boost::signals2::signal<void(std::string)> messageReceived;
};