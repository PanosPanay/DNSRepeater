// DNSRepeater.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "../DBMS/dbms.h"
#include "repeater.h"
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

using namespace std;

#define defaultNS "202.106.0.20"				//默认名字服务器
#define defaultInitFileName "dnsrelay.txt"		//默认配置文件

int main(int argc, char* argv[])
{
	string initFileName;						//配置文件
	ipv4_t nameSever;							//外部dns服务器

	//3种命令行语法
	if (argc == 1)								//dnsrelay	
	{
		initFileName = defaultInitFileName;
		nameSever = inet_addr(defaultNS);
	}
	else if (argc == 3)							//dnsrelay -dd 202.99.96.68
	{
		initFileName = defaultInitFileName;
		nameSever = inet_addr(argv[2]);
	}
	else if (argc == 4)							//dnsrelay -d 192.168.0.1 c:\dns-table.txt
	{
		initFileName = argv[3];
		nameSever = inet_addr(argv[2]);
	}

	initSet(initFileName);						//将配置文件导入域名解析数据库

	DNSRepeater repeater(inet_addr("127.0.0.1"));
	while (1)
	{
		repeater.Run();
	}

	return 0;
}

//将初始配置文件导入域名解析数据库
int initSet(string fileName)
{
	ifstream initFile(fileName.c_str(), ios::in);

	//文件打开失败
	if (!initFile)
	{
		return -1;
	}

	//文件打开成功
	else
	{
		while (!initFile.eof())
		{
			string IP, domain;
			initFile >> IP >> domain;
			if (IP != "" && domain != "")
			{
				//插入数据库
				//insert
				///////////////////////////////////////////////////////
			}
		}
		initFile.close();
	}
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
