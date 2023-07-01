#include "common.h"

std::thread StartMySqlTask()
{
	//一分钟执行一次的子线程
	std::thread MySqlThread([&]() {
		while (true)
		{
			MYSQL mysql;            //数据库句柄
			MYSQL_RES* res = NULL;   //查询结果集
			MYSQL_ROW row;         //记录结构体

			//初始化数据库
			mysql_init(&mysql);

			//设置字符集   "gbk"  字符集
			mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

			//连接数据库
			/* ln123456789   是你的数据库密码     school 是创建的一个数据库,  当然其他费参数是可以修改的*/
			if (mysql_real_connect(&mysql, "127.0.0.1", "root", "qwer1234", "db0625", 3306, NULL, 0) == NULL) {
				printf("错误原因:%s\n", mysql_error(&mysql));
				printf("连接失败!\n");
				exit(-1);
			}
			else {
				printf("连接成功\n");
			}
			//查询数据
			// 获取当前日期
			std::time_t currentTime = std::time(nullptr);
			std::tm* localTime = std::localtime(&currentTime);
			int year = localTime->tm_year + 1900; // 年份需要加上 1900
			int month = localTime->tm_mon + 1; // 月份范围为 0-11，需要加 1
			int day = localTime->tm_mday;

			// 拼接查询语句
			std::stringstream ss;
			ss << "SELECT msg FROM table_" << year << "_" << month << "_" << day;
			std::string sql = ss.str();


			int ret = mysql_query(&mysql, sql.c_str());
			printf("ret :%d\n", ret);

			//获取结果集
			res = mysql_store_result(&mysql);

			boost::unique_lock<boost::shared_mutex> lock(logMutex);

			if (res != nullptr)
			{

				std::ofstream this_ofs("test_log/this.log", std::ios::out | std::ios::trunc);

				if (this_ofs.is_open())
				{
					// 逐行写入结果到文件
					while ((row = mysql_fetch_row(res)))
					{
						this_ofs << row[0] << "\n"; // 将结果写入文件
					}
					this_ofs.close();
					std::cout << "Results written to file." << std::endl;
				}
				else
				{
					std::cout << "Failed to open file for writing." << std::endl;
				}
				//释放结果集
				mysql_free_result(res);
			}
			else
			{
				std::cout << "Failed to get result." << std::endl;
			}

			// 获取过往三天的日期
			std::tm pastTime = *localTime;
			pastTime.tm_mday -= 3; // 减去3天
			std::mktime(&pastTime);

			// 查询过往三天的数据
			std::ofstream all_ofs("test_log/all.log", std::ios::out | std::ios::trunc);
			if (all_ofs.is_open())
			{
				for (int i = 0; i < 3; ++i)
				{
					pastTime.tm_mday += 1; // 递增日期
					std::mktime(&pastTime);

					// 拼接查询语句
					std::stringstream ss;
					ss << "SELECT msg FROM table_" << pastTime.tm_year + 1900 << "_" << pastTime.tm_mon + 1 << "_" << pastTime.tm_mday;
					std::string sql = ss.str();

					// 执行查询语句
					if (mysql_query(&mysql, sql.c_str()) == 0)
					{
						// 获取结果集
						res = mysql_store_result(&mysql);
						if (res != nullptr)
						{
							// 逐行写入结果到文件
							while ((row = mysql_fetch_row(res)))
							{
								all_ofs << row[0] << "\n"; // 将结果写入文件
							}
							mysql_free_result(res);
						}
					}
				}

				all_ofs.close();
				std::cout << "Results written to file." << std::endl;
			}
			else
			{
				std::cout << "Failed to open file for writing." << std::endl;
			}

			lock.unlock();

			//关闭数据库
			mysql_close(&mysql);


			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
		});
		return MySqlThread;
}

void InitConfigMap(std::unordered_map<std::string, std::string>& configMap, SystemConfig& configNames)
{
	configMap.clear();
	
	std::list<std::string> configList = configNames.GetConfigList();
	std::list<std::string>::iterator it = configList.begin();

	for (; it!= configList.end(); it++)
	{
		configMap[*it] = "";
	}
}

void ReadSystemConfig(std::unordered_map<std::string, std::string>& configMap, SystemConfig& configNames)
{
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("System.ini", pt);

	// Reading values
	std::unordered_map<std::string, std::string>::iterator it = configMap.begin();

	for (; it != configMap.end(); it++)
	{
		it->second = pt.get<std::string>(it->first);
		std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
	}

	// Modify values
	//pt.put("HTTP.port", 8080);
	//pt.put("HTTP.topic", "new_topic");
	//pt.put("ZeroMQ.ipc", "new_ipc");
	//pt.put("ZeroMQ.topic", "new_zmq_topic");
	//
	//boost::property_tree::ini_parser::write_ini("System.ini", pt);
}