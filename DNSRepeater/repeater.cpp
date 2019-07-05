#include "pch.h"
#include "repeater.h"
#include "../DBMS/dbms.h"
#include "Windows.h"

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
	std::list<DNSCom::message_t::question_t>::iterator qListIt;
	int blockedFlag = 0;										//Ϊ1����������һ��question��ѯ�����������Σ�ֱ�ӻ�rcode=3
	int notFoundFlag = 0;										//Ϊ1����������һ��question��ѯ�����������ݿ��в��Ҳ�����ֱ��ת����ʵ�ʵı���DNS������

	while (_success)
	{
		RecvMsg = _com.RecvFrom();								//������Ϣ��
		switch (RecvMsg.type)
		{
		case DNSCom::message_t::type_t::RECV:					//DNS�������յ�����Ϣ���Ͷ���RECV
			switch (RecvMsg.header.qr)							//�ж��ǲ�ѯ�����ģ�0��������Ӧ���ģ�1��
			{
			case 0:												//0��ʾ�ǲ�ѯ������
				blockedFlag = 0;
				notFoundFlag = 0;
				//��ÿһ��question����������DNS���ݿ�
				for (qListIt = RecvMsg.qs.begin(); qListIt!=RecvMsg.qs.end() && blockedFlag == 0; ++qListIt)
				{
					DNSCom::message_t::question_t question = *qListIt;
					std::list<DNSCom::message_t::answer_t> answers;	//һ��question�����ж��answers
					DNSDBMS dbms;

					answers = dbms.Select(question);			//��ѯ���ݿ�				
					
					if (answers.size() > 0)						//��ͨIP��ַ or IP��ַΪ0.0.0.0
					{
						RecvMsg.header.ancount += answers.size();
						for (std::list<DNSCom::message_t::answer_t>::iterator aListIt = answers.begin();
							aListIt != answers.end() && blockedFlag == 0; ++aListIt)
						{
							RecvMsg.as.push_back(*aListIt);

							if (aListIt->ipv4 == inet_addr("0.0.0.0"))	//IP��ַΪ0.0.0.0
							{
								blockedFlag = 1;
							}
						}
					}
					else if (answers.size() == 0)				//δ������������,��ʵ�ʵı���DNS������ת����ѯ����
					{
						notFoundFlag = 1;
					}
				}
				if (blockedFlag == 1)							//������һ��question��ѯ�����������Σ�ֱ�ӻ�rcode=3
				{
					SendMsg = RecvMsg;

					SendMsg.header.ancount = 0;
					SendMsg.as.clear();							//���answer��

					SendMsg.header.rcode = 3;					//��ʾ���������ڣ����Σ�
					SendMsg.header.qr = 1;						//��Ӧ

					SendMsg.ipv4 = RecvMsg.ipv4;				//��Ӧ���ͻ���
				}
				else if (notFoundFlag == 1)						//������һ��question��ѯ�������鲻������û�����������Σ�ֱ��ת����ʵ�ʵı���DNS������
				{
					SendMsg = RecvMsg;

					SendMsg.header.ancount = 0;
					SendMsg.as.clear();							//���answer��

					//����ID�����浽ӳ���
					id_t pairID = _resolvers.size();
					recvPair.first = RecvMsg.ipv4;
					recvPair.second = RecvMsg.header.id;
					_resolvers.insert(std::pair<id_t, std::pair<ipv4_t, id_t>>(pairID, recvPair));
					SendMsg.header.id = pairID;

					//ת����ʵ�ʵı���DNS������
					SendMsg.ipv4 = _localDnsServer;

					//�ټ��ϼ�ʱ���Գ�ʱ���д���////////////////////////////////////////////////
				}
				else if (blockedFlag == 0 && notFoundFlag == 0)	//����question�����������ݿⶼ����ͨip��ַ
				{
					SendMsg = RecvMsg;
					
					SendMsg.header.rcode = 0;					//��Ӧ����û�в��
					SendMsg.header.qr = 1;						//��Ӧ
				}
				break;
			case 1:												//1��ʾ�������ⲿDNS����������Ӧ����
				//ת����Ӧ�ؿͻ��ˣ�ͨ��RecvMsg.header.id��ȷ����Ӧ���ѯ�����Ƿ�ƥ��	
				mapIt = _resolvers.find(RecvMsg.header.id);
				if (mapIt != _resolvers.end())					//�鵽
				{
					recvPair = _resolvers[RecvMsg.header.id];	//ͨ��RecvMsg.header.id�õ�pair
					SendMsg = RecvMsg;
					SendMsg.ipv4 = recvPair.first;				//ͨ��pair�ĵ�ַ�޸�ת����Ϣ����ip
					_resolvers.erase(mapIt);					//�Ѿ�ת���ظ��ͻ��ˣ�ɾ��ӳ������
				}
				//�Բ�ѯ���Ľ���������ݿ⣿/////////////////////////////////(������ݿ��в�������ͬ������룬������ͬ�������TTL)
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		_com.SendTo(SendMsg);									//������Ϣ��
	}
}

void DNSRepeater::Stop()
{
	_success = false;
}
