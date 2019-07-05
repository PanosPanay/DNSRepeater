#include <iostream>
#include "dbms.h"
#include "string"
#include <Windows.h>

using namespace std;

DNSDBMS::DNSDBMS():
	_env(NULL),_con(NULL)
{
}

DNSDBMS::~DNSDBMS()
{
	Disconnect();
}

bool DNSDBMS::Connect()													//与ODBC数据源连接
{
	SQLRETURN ret = SQLAllocEnv(&_env);			                        //初始化ODBC环境
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		ret = SQLAllocConnect(_env, &_con);								//分配连接句柄
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			SQLCHAR dataSource[] = "DNSDB";								//数据源
			SQLCHAR username[] = "sa";									//用户名
			SQLCHAR password[] = "wbj321...";							//密码
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

//查询函数
std::list<DNSCom::message_t::answer_t> DNSDBMS::Select(DNSCom::message_t::question_t question)
{
	Connect();															//与ODBC建立连接

	char sql[0xFF];
	SQLHSTMT stm;														//语句句柄
	SQLBIGINT ttl;														//查询到的ttl
	SQLBIGINT preference;												//查询到的preference
	char dnsvalue[100];													//查询到的dnsvalue
	std::list<DNSCom::message_t::answer_t> answers;						//返回的查询结果

	std::sprintf(sql,
		"select TTL, preference, dnsvalue from DNS where dnsname=%ls and dnsclass=%I64d and dnstype=%I64d",
		question.name,
		question.cls,
		question.dnstype);

	_protection.lock();

	SQLRETURN ret = SQLAllocStmt(_con, &stm);							//为语句句柄分配内存
	//将数据缓冲区绑定数据库中的相应字段(第二个参数代表列号)
	ret = SQLBindCol(stm, 2, SQL_C_SBIGINT, &ttl, 0, 0);				//对整数，驱动程序会忽略BufferLength并假定缓冲区足够大以保存数据
	ret = SQLBindCol(stm, 5, SQL_C_SBIGINT, &preference, 0, 0);
	ret = SQLBindCol(stm, 6, SQL_C_CHAR, dnsvalue, _countof(dnsvalue), 0);

	//遍历结果到相应缓冲区
	while (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		ret = SQLFetch(stm);
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			DNSCom::message_t::answer_t ans;
			ans.name = question.name;
			ans.dnstype = question.dnstype;
			ans.cls = question.cls;
			ans.ttl = ttl;

			if (question.dnstype == DNSCom::message_t::dns_t::A)		//A模式
			{
				ans.ipv4 = inet_addr(dnsvalue);
			}

			else														//其他模式：MX,CNAME,NS等
			{
				string str;
				if (question.dnstype == DNSCom::message_t::dns_t::MX)	//MX模式
				{
					str = to_string(preference);
					str += " ";
				}

				str += dnsvalue;
				ans.str = str;
			}

			answers.push_back(ans);
		}
	}
	ret = SQLFreeStmt(stm, SQL_DROP);

	_protection.unlock();

	Disconnect();														//与ODBC断开连接

	return answers;
}

//插入函数
void DNSDBMS::Insert(string name, int32_t ttl, DNSCom::message_t::class_t cls, DNSCom::message_t::dns_t type, int preference, string value)
{
	Connect();															//与ODBC建立连接

	SQLCHAR sql[0xFF];
	SQLHSTMT stm;														//语句句柄
	std::sprintf((char*)(sql),
		"insert into DNS values(%ls, %I64d, %I64d, %I64d, %I64d, %ls)",
		name,
		ttl,
		cls,
		type,
		preference,
		value);

	_protection.lock();

	SQLRETURN ret = SQLAllocStmt(_con, &stm);							//为语句句柄分配内存
	ret = SQLExecDirect(stm, sql, SQL_NTS);
	ret = SQLFreeStmt(stm, SQL_DROP);

	_protection.unlock();

	Disconnect();														//与ODBC断开连接
}

//清空数据库函数
void DNSDBMS::Clear()
{
	Connect();															//与ODBC建立连接

	SQLCHAR sql[0xFF] = "delete from DNS";
	SQLHSTMT stm;

	_protection.lock();

	SQLRETURN ret = SQLAllocStmt(_con, &stm);							//为语句句柄分配内存
	ret = SQLExecDirect(stm, sql, SQL_NTS);
	ret = SQLFreeStmt(stm, SQL_DROP);

	_protection.unlock();

	Disconnect();														//与ODBC断开连接
}

