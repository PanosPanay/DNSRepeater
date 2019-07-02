#pragma once

#include <sqlext.h>

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
};