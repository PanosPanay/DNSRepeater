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

	void Select(SQLVARCHAR name, SQLBIGINT sort);
	void Insert(SQLVARCHAR name, SQLBIGINT ttl, SQLBIGINT cls, SQLBIGINT type, SQLBIGINT preference, SQLVARCHAR value);

private:
	SQLHENV _env;			//�������
	SQLHDBC _con;			//���Ӿ��
	std::mutex _protection;	
};