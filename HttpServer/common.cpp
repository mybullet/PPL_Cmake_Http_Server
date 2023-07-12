#include "common.h"

std::thread StartMySqlTask()
{
	//һ����ִ��һ�ε����߳�
	std::thread MySqlThread([&]() {
		while (true)
		{
			MYSQL mysql;            //���ݿ���
			MYSQL_RES* res = NULL;   //��ѯ�����
			MYSQL_ROW row;         //��¼�ṹ��

			//��ʼ�����ݿ�
			mysql_init(&mysql);

			//�����ַ���   "gbk"  �ַ���
			mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

			//�������ݿ�
			/* ln123456789   ��������ݿ�����     school �Ǵ�����һ�����ݿ�,  ��Ȼ�����Ѳ����ǿ����޸ĵ�*/
			if (mysql_real_connect(&mysql, "mysql-server", "root", "qwer1234", "db0625", 3306, NULL, 0) == NULL) {
				printf("error code :%s\n", mysql_error(&mysql));
				printf("fail connect mysql\n");
				//exit(-1);
				break;
			}
			else { 
				printf("success connect mysql\n");
			}
			//��ѯ����
			// ��ȡ��ǰ����
			std::time_t currentTime = std::time(nullptr);
			std::tm* localTime = std::localtime(&currentTime);
			int year = localTime->tm_year + 1900; // �����Ҫ���� 1900
			int month = localTime->tm_mon + 1; // �·ݷ�ΧΪ 0-11����Ҫ�� 1
			int day = localTime->tm_mday;

			// ƴ�Ӳ�ѯ���
			std::stringstream ss;
			ss << "SELECT msg FROM table_" << year << "_" << month << "_" << day;
			std::string sql = ss.str();


			int ret = mysql_query(&mysql, sql.c_str());
			printf("ret :%d\n", ret);

			//��ȡ�����
			res = mysql_store_result(&mysql);

			boost::unique_lock<boost::shared_mutex> lock(logMutex);

			if (res != nullptr)
			{

				std::ofstream this_ofs("test_log/this.log", std::ios::out | std::ios::trunc);

				if (this_ofs.is_open())
				{
					// ����д�������ļ�
					while ((row = mysql_fetch_row(res)))
					{
						this_ofs << row[0] << "\n"; // �����д���ļ�
					}
					this_ofs.close();
					std::cout << "Results written to file." << std::endl;
				}
				else
				{
					std::cout << "Failed to open file for writing." << std::endl;
				}
				//�ͷŽ����
				mysql_free_result(res);
			}
			else
			{
				std::cout << "Failed to get result." << std::endl;
			}

			// ��ȡ�������������
			std::tm pastTime = *localTime;
			pastTime.tm_mday -= 3; // ��ȥ3��
			std::mktime(&pastTime);

			// ��ѯ�������������
			std::ofstream all_ofs("test_log/all.log", std::ios::out | std::ios::trunc);
			if (all_ofs.is_open())
			{
				for (int i = 0; i < 3; ++i)
				{
					pastTime.tm_mday += 1; // ��������
					std::mktime(&pastTime);

					// ƴ�Ӳ�ѯ���
					std::stringstream ss;
					ss << "SELECT msg FROM table_" << pastTime.tm_year + 1900 << "_" << pastTime.tm_mon + 1 << "_" << pastTime.tm_mday;
					std::string sql = ss.str();

					// ִ�в�ѯ���
					if (mysql_query(&mysql, sql.c_str()) == 0)
					{
						// ��ȡ�����
						res = mysql_store_result(&mysql);
						if (res != nullptr)
						{
							// ����д�������ļ�
							while ((row = mysql_fetch_row(res)))
							{
								all_ofs << row[0] << "\n"; // �����д���ļ�
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

			//�ر����ݿ�
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