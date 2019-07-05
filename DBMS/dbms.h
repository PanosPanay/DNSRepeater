#pragma once

#include <sqlext.h>
#include <mutex>
//#include "../DNSRepeater/repeater.h"
#include "../Communicator/com.h"
#include <list>

class DNSDBMS
{
public:
	DNSDBMS();
	~DNSDBMS();

public:
	bool Connect();
	void Disconnect();

	//int Select(DNSCom::message_t::question_t question, std::list<DNSCom::message_t::answer_t> &answers);
	std::list<DNSCom::message_t::answer_t> Select(DNSCom::message_t::question_t question);
	void Insert(string name, int32_t ttl, DNSCom::message_t::class_t cls, DNSCom::message_t::dns_t type, int preference, string value);
	void Clear();			//清空数据库

private:
	SQLHENV _env;			//环境句柄
	SQLHDBC _con;			//连接句柄
	std::mutex _protection;	
};