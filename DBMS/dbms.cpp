#include <iostream>
#include "dbms.h"
#include "string"
#include <vector>

using namespace std;

DNSDBMS::DNSDBMS():
	_env(NULL),_con(NULL)
{
}

DNSDBMS::~DNSDBMS()
{
	Disconnect();
}

bool DNSDBMS::Connect()							//与ODBC数据源连接
{
	SQLRETURN ret = SQLAllocEnv(&_env);			//初始化ODBC环境
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		ret = SQLAllocConnect(_env, &_con);		//分配连接句柄
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			SQLCHAR dataSource[] = "DNSDB";		//数据源
			SQLCHAR username[] = "sa";			//用户名
			SQLCHAR password[] = "wbj321...";	//密码
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

/*struct dns_t
{
	SQLVARCHAR dnsname;
	SQLBIGINT TTL;
	SQLVARCHAR dnsclass;
	SQLVARCHAR dnstype;
	SQLBIGINT preference;
	SQLVARCHAR dnsvalue;
};*/

void DNSDBMS::Select(SQLVARCHAR name, SQLBIGINT cls)
{
	char sql[0xFF];
	SQLHSTMT stm;			//语句句柄
	//dns_t dns;
	SQLCHAR value[100];		//查询到的value
	vector<string> result;

	std::sprintf(sql,
		"select value from DNS where dnsname=%ls and dnsclass=%I64d",
		name,
		cls);

	_protection.lock();

	SQLRETURN ret = SQLAllocStmt(_con, &stm);	//为语句句柄分配内存
	//将数据缓冲区绑定数据库中的相应字段(第二个参数代表列号)
	ret = SQLBindCol(stm, 16, SQL_C_CHAR, value, _countof(value), 0);

	//遍历结果到相应缓冲区
	while (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		ret = SQLFetch(stm);
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			string valueStr = 
			result.pushback(valueStr)
		}
	}
	ret = SQLFreeStmt(stm, SQL_DROP);

	_protection.unlock();

	//return data?
}

void DNSDBMS::Insert(SQLVARCHAR name, SQLBIGINT ttl, SQLBIGINT cls, SQLBIGINT type, SQLBIGINT preference, SQLVARCHAR value)
{
	SQLCHAR sql[0xFF];
	SQLHSTMT stm;	//语句句柄
	std::sprintf((char*)(sql),
		"insert into DNS values(%ls,%I64d,%I64d,%I64d,%I64d,%ls)",
		name,
		ttl,
		cls,
		type,
		preference,
		value);

	_protection.lock();

	SQLRETURN ret = SQLAllocStmt(_con, &stm);	//为语句句柄分配内存
	ret = SQLExecDirect(stm, sql, SQL_NTS);
	ret = SQLFreeStmt(stm, SQL_DROP);

	_protection.unlock();
}

