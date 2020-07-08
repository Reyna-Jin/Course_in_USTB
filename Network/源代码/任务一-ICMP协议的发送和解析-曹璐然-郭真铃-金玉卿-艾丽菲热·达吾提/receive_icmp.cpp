#define WIN32
#include "pcap.h"
#pragma comment ( lib, "wpcap.lib")
#include "stdio.h"
#include "time.h"
#pragma warning (disable: 4996)

using namespace std;
typedef struct ip_address { // 4 字节的 ip 地址
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;
// ipv4 首部
typedef struct ip_header {
	u_char ver_ihl; // 版本 (4 bits) + 首部长度 (4 bits)
	u_char tos; // 服务类型(8 bits)
	u_short tlen; // 总长(16 bits)
	u_short identification; // 标识(16 bits)
	u_short flags_fo; // 标志位(3 bits) + 段偏移量 (13 bits)
	u_char ttl; // 存活时间(8 bits)
	u_char proto; // 协议(8 bits)
	u_short crc; // 首部校验和(16 bits)
	ip_address saddr; // 源地址(32 bits)
	ip_address daddr; // 目的地址(32 bits)
	u_int op_pad; // 选项与填充(32 bits)
};
typedef struct icmp_header
{
	u_char type;         // 类型(8 bit)
	u_char code;         //代码(8 bit)
	u_short checksum;    //校验和(16 bits)
	u_short id;          // 标识符 (16 bits)
	u_short seq;         // 序列号 (16 bits)

};
typedef struct mac_address {
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
	u_char byte5;
	u_char byte6;
};
typedef struct MAC_header {
	mac_address daddr;// 目的地址(48 bits)
	mac_address saddr; // 源地址(48 bits)
	u_short type; //类型（16 bit）

};

void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data); // 回调函数原型
void write_log(char* timestr, const struct pcap_pkthdr* header, MAC_header* mah, ip_header* ih, icmp_header* ich);//写入日志文件
	// 主函数
int main()
{
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int inum;
	int i = 0;
	pcap_t* adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	char packet_filter[] = "icmp and ip"; //packet_filter：过滤规则表达式
	struct bpf_program fcode;
	;
	//将PCAP_SRC_IF_STRING转换为char*
	char* tmp = new char[strlen(PCAP_SRC_IF_STRING) + 1];
	strcpy(tmp, PCAP_SRC_IF_STRING);
	//
	if (pcap_findalldevs_ex(tmp, NULL, &alldevs, errbuf) == -1)
		// 获得设备列表
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}
	for (d = alldevs; d; d = d->next)
	{ // 打印网络适配器列表
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}
	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}
	printf("Enter the interface number (1-%d):", i);
	scanf_s("%d", &inum); //scanf C语言的标准输入函数
	if (inum < 1 || inum > i) //判断输入是否正确 是否是1-i之间的数字
	{
		printf("\nInterface number out of range.\n");
		pcap_freealldevs(alldevs); //释放设备列表
		return -1;
	}
	//跳转到已选网络适配卡
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);
	if ((adhandle = pcap_open(d->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf)) == NULL)//打开网络适配卡
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcapn");
		pcap_freealldevs(alldevs); // 释放设备列表
		return -1;
	}
	//// 检查数据链路层，只考虑以太网
	//if (pcap_datalink(adhandle) != DLT_EN10MB)
	//{
	//	fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
	//	pcap_freealldevs(alldevs); // 释放设备列表
	//	return -1;
	//}
	if (d->addresses != NULL)
		// 获得接口第一个地址的网络掩码
		netmask = ((struct sockaddr_in*) (d->addresses->netmask))->sin_addr.S_un.S_addr;
	else //如果接口没有地址， 则假设一个 C 类的掩码
		netmask = 0xffffff;
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0) //编译过滤器  执行成功返回 0，否则返回-1。 //fcode：pcap_compile（）返回结果
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		pcap_freealldevs(alldevs); // 释放设备列表
		return -1;
	}
	if (pcap_setfilter(adhandle, &fcode) < 0) //设置过滤器
	{
		fprintf(stderr, "nError setting the filter.\n");
		pcap_freealldevs(alldevs); // 释放设备列表
		return -1;
	}
	printf("\nlistening on %s...\n", d->description);
	pcap_freealldevs(alldevs); // 释放设备列表
	pcap_loop(adhandle, 0, packet_handler, NULL); // 开始捕捉 用回调方式捕获数据包
	return 0;
}

/*
回调函数，当收到每一个数据包时会被 libpcap 所调用
param： pcap_loop 函数传导的 user；
header： 捕获数据包的首部指针；
pkt_data ： 捕获数据包的内容。
*/
void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data)
{
	struct tm* ltime;
	char timestr[16];
	ip_header* ih;
	icmp_header* ich;
	u_int ip_len;
	MAC_header* mah = (MAC_header*)pkt_data;
	u_short sport, //源端口
		dport; //目的端口
	time_t local_tv_sec;
	local_tv_sec = header->ts.tv_sec; //将时间戳转换成可识别的格式
	/*
	c 库函数 struct tm *localtime(const time_t *timer) 使用 timer 的值来填充 tm 结构。timer 的值被分解为 tm 结构，并用本地时区表示。
	timer -- 这是指向表示日历时间的 time_t 值的指针。
	返回值：该函数返回指向 tm 结构的指针，该结构带有被填充的时间信息。下面是 tm 结构的细节：
	struct tm {
	   int tm_sec;         //秒，范围从 0 到 59
		int tm_min;         //分，范围从 0 到 59
		int tm_hour;         // 小时，范围从 0 到 23
		int tm_mday;         // 一月中的第几天，范围从 1 到 31
		int tm_mon;          // 月份，范围从 0 到 11
		int tm_year;         // 自 1900 起的年数
		int tm_wday;         // 一周中的第几天，范围从 0 到 6
		int tm_yday;         // 一年中的第几天，范围从 0 到 365
		int tm_isdst;        // 夏令时
	};
	*/
	ltime = localtime(&local_tv_sec);
	/*
	c 库函数 size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *timeptr) 根据 format 中定义的格式化规则，格式化结构 timeptr 表示的时间，并把它存储在 str 中。
	str -- 这是指向目标数组的指针，用来复制产生的 c 字符串。
	maxsize -- 这是被复制到 str 的最大字符数。
	format -- 这是 c 字符串，包含了普通字符和特殊格式说明符的任何组合。这些格式说明符由函数替换为表示 tm 中所指定时间的相对应值。格式说明符：
	https://www.runoob.com/cprogramming/c-function-strftime.html
	如果产生的 c 字符串小于 size 个字符（包括空结束字符），则会返回复制到 str 中的字符总数（不包括空结束字符），否则返回零。
	*/
	strftime(timestr, sizeof timestr, "%H:%M:%S", ltime); //打印 小时：分钟：秒
	//打印数据包的时间戳和长度
	printf("%s.%.6d len:%d ", timestr, header->ts.tv_usec, header->len);

	//获得 IP 数据包头部的位置
	ih = (ip_header*)(pkt_data + 14);// 收到的数据包是MAC帧，MAC帧的头部为14个字节（目的地址-6字节 --> 源地址-6字节 --> 类型-2字节），然后才是mac帧的数据部分（即开始ip数据报）
	ip_len = (ih->ver_ihl & 0xf) * 4; //得到ip的首部长度*4（因为首部长度的单位是4字节） 最终得到ip数据报以字节为单位的首部的长度
	ich = (icmp_header*)((u_char*)ih + ip_len); // 获得icmp首部的位置
	 //将网络字节序列转换成主机字节序列

	//打印 IP 地址和 tcp 端口
	printf("%d.%d.%d.%d -> %d.%d.%d.%d\n",
		ih->saddr.byte1,
		ih->saddr.byte2,
		ih->saddr.byte3,
		ih->saddr.byte4,
		ih->daddr.byte1,
		ih->daddr.byte2,
		ih->daddr.byte3,
		ih->daddr.byte4
	);
	write_log(timestr, header, mah, ih, ich);
}

//将结果写入日志文件
void write_log(char* timestr, const struct pcap_pkthdr* header, MAC_header* mah, ip_header* ih, icmp_header*ich) {
	FILE* fp;
	fp = fopen("Receive.log", "a");//在当前程序的目录下创建和追加用于读写的空的日志文件：Receive.log
	fprintf(fp, "%s.%.6d len:%d \n###MAC首部###\n目的MAC地址：%X:%X:%X:%X:%X:%X \n 源MAC地址： %X:%X:%X:%X:%X:%X \n类型：%X\n",
		timestr,
		header->ts.tv_usec,
		header->len,
		mah->daddr.byte1,
		mah->daddr.byte2,
		mah->daddr.byte3,
		mah->daddr.byte4,
		mah->daddr.byte5,
		mah->daddr.byte6,
		mah->saddr.byte1,
		mah->saddr.byte2,
		mah->saddr.byte3,
		mah->saddr.byte4,
		mah->saddr.byte5,
		mah->saddr.byte6,
		mah->type
	);
	u_char version = ih->ver_ihl >> 4;//拆分版本
	u_char head_len = ih->ver_ihl & 0x0f;//拆分首部长度
	u_char flag = ih->flags_fo >> 13;//标志
	u_short mov_to = ih->flags_fo & 0x1fff;//片偏移
	char* s;
	//拆分协议

	switch (ih->proto)
	{
	case 1:
		s = new char[strlen("ICMP") + 1];
		strcpy(s, "ICMP");
		break;
	case 2:
		s = new char[strlen("IGMP") + 1];
		strcpy(s, "IGMP");
		break;
	case 4:
		s = new char[strlen("IP") + 1];
		strcpy(s, "IP");
		break;
	case 6:
		s = new char[strlen("TCP") + 1];
		strcpy(s, "TCP");
		break;
	case 8:
		s = new char[strlen("EGP") + 1];
		strcpy(s, "EGP");
		break;
	case 9:
		s = new char[strlen("IGP") + 1];
		strcpy(s, "IGP");
		break;
	case 17:
		s = new char[strlen("UDP") + 1];
		strcpy(s, "UDP");
		break;
	case 41:
		s = new char[strlen("IPv6") + 1];
		strcpy(s, "IPv6");
		break;
	case 50:
		s = new char[strlen("ESP") + 1];
		strcpy(s, "ESP");
		break;
	case 89:
		s = new char[strlen("OSPF") + 1];
		strcpy(s, "OSPF");
		break;
	default:
		s = new char[strlen("Unkown") + 1];
		strcpy(s, "Unkown");
		break;
	}

	fprintf(fp, "###IP数据报首部###\n版本：%d\n首部长度：%d\n总长度：%d字节\n标识：%x\n标志：%x\n片偏移：%0x(*8字节)\n生存时间：%d\n协议：%s\n首部校验和：%x\n源地址：%d.%d.%d.%d\n目的地址：%d.%d.%d.%d\n",
		version,
		head_len,
		ih->tlen,
		ih->identification,
		flag,
		mov_to,
		ih->ttl,
		s,
		ih->crc,
		ih->saddr.byte1,
		ih->saddr.byte2,
		ih->saddr.byte3,
		ih->saddr.byte4,
		ih->daddr.byte1,
		ih->daddr.byte2,
		ih->daddr.byte3,
		ih->daddr.byte4
	);

	switch (ich->type)
	{
	case 0:
		s = new char[strlen("查询报文----应答") + 1];
		strcpy(s, "查询报文----应答");
		break;
	case 3:
		s = new char[strlen("差错报告报文----目的不可达") + 1];
		strcpy(s, "差错报告报文----目的不可达");
		break;
	case 4:
		s = new char[strlen("控制报文----源抑制") + 1];
		strcpy(s, "控制报文----源抑制");
		break;
	case 5:
		s = new char[strlen("控制报文----路由重定向") + 1];
		strcpy(s, "控制报文----路由重定向");
		break;
	case 8:
		s = new char[strlen("查询报文----回送请求") + 1];
		strcpy(s, "查询报文----回送请求");
		break;
	case 10:
		s = new char[strlen("查询报文----路由器请求") + 1];
		strcpy(s, "查询报文----路由器请求");
		break;
	case 11:
		s = new char[strlen("差错报告报文----超时") + 1];
		strcpy(s, "差错报告报文----超时");
		break;
	case 12:
		s = new char[strlen("差错报告报文----参数出错") + 1];
		strcpy(s, "差错报告报文----参数出错");
		break;
	case 13:
		s = new char[strlen("查询报文----时间戳请求") + 1];
		strcpy(s, "查询报文----时间戳请求");
		break;
	case 14:
		s = new char[strlen("查询报文----时间戳应答") + 1];
		strcpy(s, "查询报文----时间戳应答");
		break;
	case 17:
		s = new char[strlen("查询报文----掩码请求") + 1];
		strcpy(s, "查询报文----掩码请求");
		break;
	case 18:
		s = new char[strlen("查询报文----掩码应答") + 1];
		strcpy(s, "查询报文----掩码应答");
		break;
	case 19:
		s = new char[strlen("差错报告报文----通告") + 1];
		strcpy(s, "差错报告报文----通告");
		break;
	default:
		s = new char[strlen("Unkown") + 1];
		strcpy(s, "Unkown");
		break;
	}
	fprintf(fp, "###ICMP首部###\n类型：%s\n代码：%x\n校验和：%x\n",
		s,
		ich->code,
		ich->checksum
	);
	fprintf(fp, "=================================================================\n");
	fclose(fp);
}