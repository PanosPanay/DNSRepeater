#include <iostream>
#include "dbms.h"

//using namespace std;

DNSDBMS::DNSDBMS():
	_env(NULL),_con(NULL)
{
}

DNSDBMS::~DNSDBMS()
{
	Disconnect();
}

bool DNSDBMS::Connect()							//��ODBC����Դ����
{
	SQLRETURN ret = SQLAllocEnv(&_env);			//��ʼ��ODBC����
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		ret = SQLAllocConnect(_env, &_con);		//�������Ӿ��
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			SQLCHAR dataSource[] = "DNSDB";		//����Դ
			SQLCHAR username[] = "sa";			//�û���
			SQLCHAR password[] = "wbj321...";	//����
			ret = SQLConnect(
				_con,
				dataSource,
				SQL_NTS,
				username,
				SQL_NTS,
				password,
				SQL_NTS
			);

			if (ret = SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			{
				return true;
			}
			SQLFreeConnect(_con);
		}
		SQLFreeEnv(_env);
	}
	return false;
}

void DNSDBMS::Disconnect()
{
	if (_con != NULL)
	{
		SQLDisconnect(_con);
		SQLFreeConnect(_con);
		_con = NULL;
	}
	if (_env != NULL)
	{
		SQLFreeEnv(_env);
		_env = NULL;
	}
}

struct dns_t
{
	SQLVARCHAR dnsname;
	SQLBIGINT TTL;
	SQLVARCHAR dnsclass;
	SQLVARCHAR dnstype;
	SQLBIGINT preference;
	SQLVARCHAR dnsvalue;
};

void DNSDBMS::Select(SQLVARCHAR name, SQLVARCHAR cls)
{
	char sql[0xFF];
	SQLHSTMT stm;	//�����
	dns_t dns;

	std::sprintf(sql,
		"select value from DNS where dnsname=%ls and dnsclass=%ls",
		name,
		cls);

	
}