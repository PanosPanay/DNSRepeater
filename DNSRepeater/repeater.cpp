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
		RecvMsg = _com.RecvFrom();							//������Ϣ��
		switch (RecvMsg.type)
		{
		case DNSCom::message_t::type_t::RECV:				//DNS�������յ�����Ϣ���Ͷ���RECV
			switch (RecvMsg.header.qr)						//�ж��ǲ�ѯ�����ģ�0��������Ӧ���ģ�1��
			{
			case 0:											//0��ʾ�ǲ�ѯ������
				//��ÿһ��question����������DNS���ݿ�
				for (int i = 0; i < RecvMsg.header.qdcount; ++i, ++listIt)
				{
					DNSCom::message_t::question_t question = *listIt;
					if ()										//��ͨIP��ַ
					{
						SendMsg = RecvMsg;

						SendMsg.header.rcode = 0;				//��Ӧ����û�в��
						SendMsg.header.qr = 1;					//��Ӧ

						//���ݲ�ѯ���Ľ������SendMsg.header.ancount

					}
					else if ()									//IP��ַΪ0.0.0.0
					{
						SendMsg = RecvMsg;

						SendMsg.header.rcode = 3;				//��ʾ���������ڣ����Σ�
						SendMsg.header.qr = 1;					//��Ӧ

						//answer
						SendMsg.header.ancount = 1;				//answer����
						DNSCom::message_t::answer_t ans;
						//ans.name=RecvMsg.qs()
						SendMsg.as.push_back(ans);

						//SendMsg.ipv4 = RecvMsg.ipv4;			//��Ӧ���ͻ���
					}
					else if ()									//δ������������,��ʵ�ʵı���DNS������ת����ѯ����
					{
						SendMsg = RecvMsg;

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
				}
				break;
			case 1:											//1��ʾ�������ⲿDNS����������Ӧ����
				//ת����Ӧ�ؿͻ��ˣ�ͨ��RecvMsg.header.id��ȷ����Ӧ���ѯ�����Ƿ�ƥ��	

				//std::map<id_t, std::pair<ipv4_t, id_t>>::iterator it;
				mapIt = _resolvers.find(RecvMsg.header.id);
				if (mapIt != _resolvers.end())					//�鵽
				{
					//std::pair<ipv4_t, id_t> recvPair;
					recvPair = _resolvers[RecvMsg.header.id];	//ͨ��RecvMsg.header.id�õ�pair
					SendMsg = RecvMsg;
					SendMsg.ipv4 = recvPair.first;			//ͨ��pair�ĵ�ַ�޸�ת����Ϣ����ip
					_resolvers.erase(mapIt);					//�Ѿ�ת���ظ��ͻ��ˣ�ɾ��ӳ������
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		_com.SendTo(SendMsg);								//������Ϣ��
	}
}

void DNSRepeater::Stop()
{
	_success = false;
}
