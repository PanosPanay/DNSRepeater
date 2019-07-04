#pragma once

/// <summary>
/// 字符串数据的最大长度
/// </summary>
#define STR_MAXN 0x40

/// <summary>
/// 内存中缓存的最大数目（目前暂不考虑实现）
/// </summary>
#define CACHE_MAXN 0xFF

/// <summary>
/// 32位IPv4地址（小端存储）
/// </summary>
typedef int32_t ipv4_t;

/// <summary>
/// 16位事务标识
/// </summary>
typedef int16_t id_t;

/// <summary>
/// 业务层
/// 
/// </summary>
class DNSRepeater
{
public:
	DNSRepeater(ipv4_t _local);
	virtual ~DNSRepeater();

/// <summary>
/// 以下为弃用函数
/// </summary>
public:
	DNSRepeater(const DNSRepeater&) = delete;
	DNSRepeater(DNSRepeater&&) = delete;
	DNSRepeater& operator=(const DNSRepeater&) = delete;
	DNSRepeater& operator=(DNSRepeater&&) = delete;

public:
	void Run();
	void Stop();

public:
	enum class opt_t
	{
		INVALID,
		QUESTION,	// DNS查询
		COMMITED	// 提交给本地DNS服务器
	};

/// <summary>
/// 数据库相关
/// </summary>
private:
	/// <summary>
	/// 资源记录类型
	/// </summary>
	struct recordv4_t
	{
		/// <summary>
		/// 
		/// </summary>
		enum class class_t
		{
			IN = 1
		};

		enum class type_t
		{
			A = 0x0001,
			NS = 0x0002,
			CNAME = 0x0005,
			SOA = 0x0006,
			MB = 0x0007,
			MG = 0x0008,
			MR = 0x0009,
			NUL = 0x000A,
			WKS = 0x000B,
			PTR = 0x000C,
			HINF = 0x000D,
			MINFO = 0x000E,
			MX = 0x000F,
			TXT = 0x0010,
			AAAA = 0x0026

		};

		std::string owner;
		int64_t ttl;
		class_t cls;
		type_t type;
		int16_t preference;
		union value_t
		{
			ipv4_t ipv4;
			char str[STR_MAXN];
		};
	};

private:
	/// <summary>
	/// 运行控制
	/// </summary>
	bool _success;

private:
	/// <summary>
	/// 实际的本地DNS服务器IPv4地址
	/// </summary>
	ipv4_t _localDnsServer;

	/// <summary>
	/// 客户端的若干解析器
	/// 唯一标志着一组IP和UDP标识所对应的操作（及目前所处的状态：正在处理、已经提交给本地DNS服务器...）
	/// 这组记录维持着通信的正常进行（UDP包的相互对应）
	/// </summary>
	//std::map<std::pair<ipv4_t, id_t>, opt_t> _resolvers; 
	std::map<id_t, std::pair<ipv4_t, id_t>> _resolvers;		//给pair<ip，id>分配一个pairId，因为可能有不同的ip地址发来的查询请求id相同，因而需要通过pair唯一地确定源。收到外部dns返回地包的id仍未pairID不变
	//std::map<std::pair<ipv4_t, id_t>, int> _timeoutHander;	//int改为time？处理求助实际本地DNS服务器但超时无应答的情况
	// std::list<recordv4_t> _cache; // 缓存（暂不考虑）

private:
	/// <summary>
	/// 通信组件
	/// </summary>
	DNSCom _com;
};