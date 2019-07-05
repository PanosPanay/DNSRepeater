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

bool DNSDBMS::Connect()													//��ODBC����Դ����
{
	SQLRETURN ret = SQLAllocEnv(&_env);			                        //��ʼ��ODBC����
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		ret = SQLAllocConnect(_env, &_con);								//�������Ӿ��
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			SQLCHAR dataSource[] = "DNSDB";								//����Դ
			SQLCHAR username[] = "sa";									//�û���
			SQLCHAR password[] = "wbj321...";							//����
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

//��ѯ����
std::list<DNSCom::message_t::answer_t> DNSDBMS::Select(DNSCom::message_t::question_t question)
{
	Connect();															//��ODBC��������

	char sql[0xFF];
	SQLHSTMT stm;														//�����
	SQLBIGINT ttl;														//��ѯ����ttl
	SQLBIGINT preference;												//��ѯ����preference
	char dnsvalue[100];													//��ѯ����dnsvalue
	std::list<DNSCom::message_t::answer_t> answers;						//���صĲ�ѯ���

	std::sprintf(sql,
		"select TTL, preference, dnsvalue from DNS where dnsname=%ls and dnsclass=%I64d and dnstype=%I64d",
		question.name,
		question.cls,
		question.dnstype);

	_protection.lock();

	SQLRETURN ret = SQLAllocStmt(_con, &stm);							//Ϊ����������ڴ�
	//�����ݻ����������ݿ��е���Ӧ�ֶ�(�ڶ������������к�)
	ret = SQLBindCol(stm, 2, SQL_C_SBIGINT, &ttl, 0, 0);				//��������������������BufferLength���ٶ��������㹻���Ա�������
	ret = SQLBindCol(stm, 5, SQL_C_SBIGINT, &preference, 0, 0);
	ret = SQLBindCol(stm, 6, SQL_C_CHAR, dnsvalue, _countof(dnsvalue), 0);

	//�����������Ӧ������
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

			if (question.dnstype == DNSCom::message_t::dns_t::A)		//Aģʽ
			{
				ans.ipv4 = inet_addr(dnsvalue);
			}

			else														//����ģʽ��MX,CNAME,NS��
			{
				string str;
				if (question.dnstype == DNSCom::message_t::dns_t::MX)	//MXģʽ
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

	Disconnect();														//��ODBC�Ͽ�����

	return answers;
}

//���뺯��
void DNSDBMS::Insert(string name, int32_t ttl, DNSCom::message_t::class_t cls, DNSCom::message_t::dns_t type, int preference, string value)
{
	Connect();															//��ODBC��������

	SQLCHAR sql[0xFF];
	SQLHSTMT stm;														//�����
	std::sprintf((char*)(sql),
		"insert into DNS values(%ls, %I64d, %I64d, %I64d, %I64d, %ls)",
		name,
		ttl,
		cls,
		type,
		preference,
		value);

	_protection.lock();

	SQLRETURN ret = SQLAllocStmt(_con, &stm);							//Ϊ����������ڴ�
	ret = SQLExecDirect(stm, sql, SQL_NTS);
	ret = SQLFreeStmt(stm, SQL_DROP);

	_protection.unlock();

	Disconnect();														//��ODBC�Ͽ�����
}

//������ݿ⺯��
void DNSDBMS::Clear()
{
	Connect();															//��ODBC��������

	SQLCHAR sql[0xFF] = "delete from DNS";
	SQLHSTMT stm;

	_protection.lock();

	SQLRETURN ret = SQLAllocStmt(_con, &stm);							//Ϊ����������ڴ�
	ret = SQLExecDirect(stm, sql, SQL_NTS);
	ret = SQLFreeStmt(stm, SQL_DROP);

	_protection.unlock();

	Disconnect();														//��ODBC�Ͽ�����
}

