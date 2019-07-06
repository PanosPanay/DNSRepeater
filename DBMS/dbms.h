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
	void Insert(std::string name, int32_t ttl, DNSCom::message_t::class_t cls, DNSCom::message_t::dns_t type, int preference, std::string value);
	void Clear();			//清空数据库
	int IsExisted(DNSCom::message_t::answer_t answer);		//数据是否已存在
	int DeleteRecod(DNSCom::message_t::answer_t answer);			//删除记录

private:
	SQLHENV _env;			//环境句柄
	SQLHDBC _con;			//连接句柄
	std::mutex _protection;	
};