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
	DNSCom::message_t RecvMsg;
	DNSCom::message_t SendMsg;
	std::pair<ipv4_t, id_t> recvPair;
	std::map<id_t, std::pair<ipv4_t, id_t>>::iterator mapIt;
	std::list<DNSCom::message_t::question_t>::iterator listIt;

	while (_success)
	{
		RecvMsg = _com.RecvFrom();							//接收消息包
		switch (RecvMsg.type)
		{
		case DNSCom::message_t::type_t::RECV:				//DNS服务器收到的消息类型都是RECV
			switch (RecvMsg.header.qr)						//判断是查询请求报文（0），或响应报文（1）
			{
			case 0:											//0表示是查询请求报文
				//对每一个question的域名检索DNS数据库
				for (int i = 0; i < RecvMsg.header.qdcount; ++i, ++listIt)
				{
					DNSCom::message_t::question_t question = *listIt;
					if ()										//普通IP地址
					{
						SendMsg = RecvMsg;

						SendMsg.header.rcode = 0;				//响应报文没有差错
						SendMsg.header.qr = 1;					//响应

						//根据查询到的结果设置SendMsg.header.ancount

					}
					else if ()									//IP地址为0.0.0.0
					{
						SendMsg = RecvMsg;

						SendMsg.header.rcode = 3;				//表示域名不存在（屏蔽）
						SendMsg.header.qr = 1;					//响应

						//answer
						SendMsg.header.ancount = 1;				//answer个数
						DNSCom::message_t::answer_t ans;
						//ans.name=RecvMsg.qs()
						SendMsg.as.push_back(ans);

						//SendMsg.ipv4 = RecvMsg.ipv4;			//回应给客户端
					}
					else if ()									//未检索到该域名,向实际的本地DNS服务器转发查询请求
					{
						SendMsg = RecvMsg;

						//分配ID，保存到映射表
						id_t pairID = _resolvers.size();
						recvPair.first = RecvMsg.ipv4;
						recvPair.second = RecvMsg.header.id;
						_resolvers.insert(std::pair<id_t, std::pair<ipv4_t, id_t>>(pairID, recvPair));
						SendMsg.header.id = pairID;

						//转发给实际的本地DNS服务器
						SendMsg.ipv4 = _localDnsServer;

						//再加上几时，对超时进行处理////////////////////////////////////////////////
					}
				}
				break;
			case 1:											//1表示是来自外部DNS服务器的响应报文
				//转发响应回客户端，通过RecvMsg.header.id来确定响应与查询请求是否匹配	

				//std::map<id_t, std::pair<ipv4_t, id_t>>::iterator it;
				mapIt = _resolvers.find(RecvMsg.header.id);
				if (mapIt != _resolvers.end())					//查到
				{
					//std::pair<ipv4_t, id_t> recvPair;
					recvPair = _resolvers[RecvMsg.header.id];	//通过RecvMsg.header.id得到pair
					SendMsg = RecvMsg;
					SendMsg.ipv4 = recvPair.first;			//通过pair的地址修改转发消息包的ip
					_resolvers.erase(mapIt);					//已经转发回给客户端，删除映射表该项
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		_com.SendTo(SendMsg);								//发送消息包
	}
}

void DNSRepeater::Stop()
{
	_success = false;
}
