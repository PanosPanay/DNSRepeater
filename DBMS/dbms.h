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
	SQLHENV _env;			//�������
	SQLHDBC _con;			//���Ӿ��
	std::mutex _protection;	
};