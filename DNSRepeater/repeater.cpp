#include "pch.h"
#include "repeater.h"

DNSRepeater::DNSRepeater(ipv4_t _local) :
	_success(false),
	_localDnsServer(_local),
	_resolvers(),
	_com(_local)
{
}

DNSRepeater::~DNSRepeater()
{
}

void DNSRepeater::Run()
{
	DNSCom::message_t msg;
	while (_success)
	{
		msg = _com.RecvFrom();
		switch (msg.type)
		{
		case DNSCom::message_t::type_t::RECV:		//收到的是来自外部DNS的response，向客户端转发response

			break;
		case DNSCom::message_t::type_t::SEND:		//收到的是来自客户端的request
			//用域名检索DNS数据库
			//select
			if ()									//普通IP地址
			{

			}
			else if ()								//0.0.0.0
			{

			}
			else if()								//未检索到域名
			{

			}
			break;
		default:
			break;
		}
	}
}

void DNSRepeater::Stop()
{
	_success = false;
}
