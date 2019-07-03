#pragma once

#include <sqlext.h>
#include <mutex>

class DNSDBMS
{
public:
	DNSDBMS();
	~DNSDBMS();

public:
	bool Connect();
	void Disconnect();

	void Select(SQLVARCHAR name, SQLVARCHAR sort);
	//void Insert()

private:
	SQLHENV _env;			//环境句柄
	SQLHDBC _con;			//连接句柄
	std::mutex _protection;	
};