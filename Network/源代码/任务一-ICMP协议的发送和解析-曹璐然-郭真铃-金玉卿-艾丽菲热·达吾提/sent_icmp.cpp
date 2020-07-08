#define WIN32
#include "pcap.h"
#include <stdlib.h>
#pragma comment ( lib, "wpcap.lib")
#include "stdio.h"
#pragma warning (disable: 4996)
#define packet_size 100
void check(u_char* packet, int len, u_char* check_num) {
	unsigned long check_sum = 0;
	for (int k = 0; k < len - 1;) {
		unsigned long tmp = (packet[k] << 8) + packet[k + 1];
		check_sum += tmp;
		k = k + 2;

	}
	//处理奇数字节情况
	if (len % 2 == 0) {
		check_sum += (packet[len - 1] << 8);
	}
	//处理进位部分
	check_sum = (check_sum >> 16) + (check_sum & 0xffff);
	check_sum += (check_sum >> 16);
	//取反 赋值
	check_num[0] = ~(u_char)(check_sum >> 8);
	check_num[1] = ~(u_char)(check_sum);
}
int main()
{
	pcap_t* fp;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_char packet[packet_size];
	int i;
	int j = 0;
	u_char check_num[2];//存放校验结果
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int inum;
	///* 检查命令行参数的合法性 */
	//if (argc != 2)
	//{
	//    printf("usage: %s interface (e.g. 'rpcap://eth0')", argv[0]);
	//    return;
	//}

	///* 打开输出设备 */
	//if ((fp = pcap_open(argv[1],            // 设备名
	//    100,                // 要捕获的部分 (只捕获前100个字节)
	//    PCAP_OPENFLAG_PROMISCUOUS,  // 混杂模式
	//    1000,               // 读超时时间
	//    NULL,               // 远程机器验证
	//    errbuf              // 错误缓冲
	//)) == NULL)
	//{
	//    fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", argv[1]);
	//    return;
	//}
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
		// 获得设备列表
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}
	for (d = alldevs; d; d = d->next)
	{ // 打印网络适配器列表
		printf("%d. %s", ++j, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}
	if (j == 0)
	{
		printf("nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}
	printf("Enter the interface number (1-%d):", j);
	scanf_s("%d", &inum); //scanf C语言的标准输入函数
	if (inum < 1 || inum > j) //判断输入是否正确 是否是1-i之间的数字
	{
		printf("nInterface number out of range.\n");
		pcap_freealldevs(alldevs); //释放设备列表
		return -1;
	}
	//跳转到已选网络适配卡
	for (d = alldevs, j = 0; j < inum - 1; d = d->next, j++);
	if ((fp = pcap_open(d->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf)) == NULL)//打开网络适配卡
	{
		fprintf(stderr, "nUnable to open the adapter. %s is not supported by WinPcapn");
		pcap_freealldevs(alldevs); // 释放设备列表
		return -1;
	}

	/*-------------封装数据--------------------------*/
/* 设置MAC目的地址为  B4-69-21-81-31-93*/
	packet[6] = 0xFF;
	packet[7] = 0xFF;
	packet[8] = 0xFF;
	packet[9] = 0xFF;
	packet[10] = 0xFF;
	packet[11] = 0xFF;

	/* 设置MAC的目的地址为 34-42-62-0E-C2-A5
	packet[0] = 0x34;
	packet[1] = 0x42;
	packet[2] = 0x62;
	packet[3] = 0x0E;
	packet[4] = 0xC2;
	packet[5] = 0xA5;*/

	/* 设置MAC源地址为  B4-69-21-81-31-93*/
	packet[6] = 0xB4;
	packet[7] = 0x69;
	packet[8] = 0x21;
	packet[9] = 0x81;
	packet[10] = 0x31;
	packet[11] = 0x93;

	/*指明上层用的是IP数据报*/
	packet[12] = 8;
	packet[13] = 0;
	/*至此MAC帧头部封装完毕*/

	/*IP数据报首部*/
	packet[14] = 0x45;//版本：4 长度：5
	packet[15] = 0x00;//区分服务 不用
	packet[16] = 0x00;//总长度：86字节 即0x0056
	packet[17] = 0x56;
	packet[18] = 0x3f;//标识：0x3ff2
	packet[19] = 0xf2;
	packet[20] = 0x00;//标志：0 这是数据报片的最后一个
	packet[21] = 0x00;//片偏移：0
	packet[22] = 0x10;//生存时间TTL
	packet[23] = 0x01;//协议：ICMP 1
	packet[24] = 0x00;//首部检验和：先置为0
	packet[25] = 0x00;
	//源地址 IP地址 192.168.0.100
	packet[26] = 192;
	packet[27] = 168;
	packet[28] = 0;
	packet[29] = 100;
	//目的地址  192.168.0.100
	//packet[30] = 192;
	//packet[31] = 168;
	//packet[32] = 0;
	//packet[33] = 100;
	// 输入目的ip地址
	printf("Please Enter the destination IP address: ");
	u_char DIPA[4] = { 0 };
	char tDIPA[16];
	scanf_s("%s", tDIPA, 16);
	int iip = 0;
	for (int i = 0; i < 16; i++)
	{
		if (tDIPA[i] == '\0')
			break;
		if (tDIPA[i] == '.')
			iip++;
		else
		{
			DIPA[iip] *= 10;
			DIPA[iip] += tDIPA[i] - 48;
		}
		if (iip == 4)
		{
			printf("Wrong ip, too many number");
			break;
		}
	}
	if (iip < 3)
	{
		printf("Wrong ip, the ip is short");
	}
	else
	{
		for (int i = 0; i < 4; i++)
			packet[30 + i] = DIPA[i];
	}
	//计算首部校验和（IP数据报）
	check(packet, 20, check_num);
	packet[24] = check_num[0];
	packet[25] = check_num[1];
	/*ICMP首部*/
	packet[34] = 0x08;//类型：询问报文 回送请求
	packet[35] = 0x00;//代码
	packet[36] = 0; //校验和
	packet[37] = 0;
	packet[38] = 0; //标识
	packet[39] = 1;
	packet[40] = 0;//序列号
	packet[41] = 0;
	/*填充剩下的内容*/
	for (i = 42; i < packet_size; i++)
	{
		packet[i] = i % 256;
	}
	
	/*计算ICMP的校验和 要包括数据在内*/
	check(packet, packet_size - 34, check_num);
	packet[36] = check_num[0];
	packet[37] = check_num[1];


	/* 发送数据包 */
	if (pcap_sendpacket(fp, packet, packet_size) != 0)
	{
		fprintf(stderr, "\nError sending the packet: \n", pcap_geterr(fp));
		return 0;
	}
	printf("Success！");
	return 0;
}