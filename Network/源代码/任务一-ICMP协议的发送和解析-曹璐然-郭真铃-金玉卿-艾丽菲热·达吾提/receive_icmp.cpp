#define WIN32
#include "pcap.h"
#pragma comment ( lib, "wpcap.lib")
#include "stdio.h"
#include "time.h"
#pragma warning (disable: 4996)

using namespace std;
typedef struct ip_address { // 4 �ֽڵ� ip ��ַ
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;
// ipv4 �ײ�
typedef struct ip_header {
	u_char ver_ihl; // �汾 (4 bits) + �ײ����� (4 bits)
	u_char tos; // ��������(8 bits)
	u_short tlen; // �ܳ�(16 bits)
	u_short identification; // ��ʶ(16 bits)
	u_short flags_fo; // ��־λ(3 bits) + ��ƫ���� (13 bits)
	u_char ttl; // ���ʱ��(8 bits)
	u_char proto; // Э��(8 bits)
	u_short crc; // �ײ�У���(16 bits)
	ip_address saddr; // Դ��ַ(32 bits)
	ip_address daddr; // Ŀ�ĵ�ַ(32 bits)
	u_int op_pad; // ѡ�������(32 bits)
};
typedef struct icmp_header
{
	u_char type;         // ����(8 bit)
	u_char code;         //����(8 bit)
	u_short checksum;    //У���(16 bits)
	u_short id;          // ��ʶ�� (16 bits)
	u_short seq;         // ���к� (16 bits)

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
	mac_address daddr;// Ŀ�ĵ�ַ(48 bits)
	mac_address saddr; // Դ��ַ(48 bits)
	u_short type; //���ͣ�16 bit��

};

void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data); // �ص�����ԭ��
void write_log(char* timestr, const struct pcap_pkthdr* header, MAC_header* mah, ip_header* ih, icmp_header* ich);//д����־�ļ�
	// ������
int main()
{
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int inum;
	int i = 0;
	pcap_t* adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	char packet_filter[] = "icmp and ip"; //packet_filter�����˹�����ʽ
	struct bpf_program fcode;
	;
	//��PCAP_SRC_IF_STRINGת��Ϊchar*
	char* tmp = new char[strlen(PCAP_SRC_IF_STRING) + 1];
	strcpy(tmp, PCAP_SRC_IF_STRING);
	//
	if (pcap_findalldevs_ex(tmp, NULL, &alldevs, errbuf) == -1)
		// ����豸�б�
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}
	for (d = alldevs; d; d = d->next)
	{ // ��ӡ�����������б�
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
	scanf_s("%d", &inum); //scanf C���Եı�׼���뺯��
	if (inum < 1 || inum > i) //�ж������Ƿ���ȷ �Ƿ���1-i֮�������
	{
		printf("\nInterface number out of range.\n");
		pcap_freealldevs(alldevs); //�ͷ��豸�б�
		return -1;
	}
	//��ת����ѡ�������俨
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);
	if ((adhandle = pcap_open(d->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf)) == NULL)//���������俨
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcapn");
		pcap_freealldevs(alldevs); // �ͷ��豸�б�
		return -1;
	}
	//// ���������·�㣬ֻ������̫��
	//if (pcap_datalink(adhandle) != DLT_EN10MB)
	//{
	//	fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
	//	pcap_freealldevs(alldevs); // �ͷ��豸�б�
	//	return -1;
	//}
	if (d->addresses != NULL)
		// ��ýӿڵ�һ����ַ����������
		netmask = ((struct sockaddr_in*) (d->addresses->netmask))->sin_addr.S_un.S_addr;
	else //����ӿ�û�е�ַ�� �����һ�� C �������
		netmask = 0xffffff;
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0) //���������  ִ�гɹ����� 0�����򷵻�-1�� //fcode��pcap_compile�������ؽ��
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		pcap_freealldevs(alldevs); // �ͷ��豸�б�
		return -1;
	}
	if (pcap_setfilter(adhandle, &fcode) < 0) //���ù�����
	{
		fprintf(stderr, "nError setting the filter.\n");
		pcap_freealldevs(alldevs); // �ͷ��豸�б�
		return -1;
	}
	printf("\nlistening on %s...\n", d->description);
	pcap_freealldevs(alldevs); // �ͷ��豸�б�
	pcap_loop(adhandle, 0, packet_handler, NULL); // ��ʼ��׽ �ûص���ʽ�������ݰ�
	return 0;
}

/*
�ص����������յ�ÿһ�����ݰ�ʱ�ᱻ libpcap ������
param�� pcap_loop ���������� user��
header�� �������ݰ����ײ�ָ�룻
pkt_data �� �������ݰ������ݡ�
*/
void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data)
{
	struct tm* ltime;
	char timestr[16];
	ip_header* ih;
	icmp_header* ich;
	u_int ip_len;
	MAC_header* mah = (MAC_header*)pkt_data;
	u_short sport, //Դ�˿�
		dport; //Ŀ�Ķ˿�
	time_t local_tv_sec;
	local_tv_sec = header->ts.tv_sec; //��ʱ���ת���ɿ�ʶ��ĸ�ʽ
	/*
	c �⺯�� struct tm *localtime(const time_t *timer) ʹ�� timer ��ֵ����� tm �ṹ��timer ��ֵ���ֽ�Ϊ tm �ṹ�����ñ���ʱ����ʾ��
	timer -- ����ָ���ʾ����ʱ��� time_t ֵ��ָ�롣
	����ֵ���ú�������ָ�� tm �ṹ��ָ�룬�ýṹ���б�����ʱ����Ϣ�������� tm �ṹ��ϸ�ڣ�
	struct tm {
	   int tm_sec;         //�룬��Χ�� 0 �� 59
		int tm_min;         //�֣���Χ�� 0 �� 59
		int tm_hour;         // Сʱ����Χ�� 0 �� 23
		int tm_mday;         // һ���еĵڼ��죬��Χ�� 1 �� 31
		int tm_mon;          // �·ݣ���Χ�� 0 �� 11
		int tm_year;         // �� 1900 �������
		int tm_wday;         // һ���еĵڼ��죬��Χ�� 0 �� 6
		int tm_yday;         // һ���еĵڼ��죬��Χ�� 0 �� 365
		int tm_isdst;        // ����ʱ
	};
	*/
	ltime = localtime(&local_tv_sec);
	/*
	c �⺯�� size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *timeptr) ���� format �ж���ĸ�ʽ�����򣬸�ʽ���ṹ timeptr ��ʾ��ʱ�䣬�������洢�� str �С�
	str -- ����ָ��Ŀ�������ָ�룬�������Ʋ����� c �ַ�����
	maxsize -- ���Ǳ����Ƶ� str ������ַ�����
	format -- ���� c �ַ�������������ͨ�ַ��������ʽ˵�������κ���ϡ���Щ��ʽ˵�����ɺ����滻Ϊ��ʾ tm ����ָ��ʱ������Ӧֵ����ʽ˵������
	https://www.runoob.com/cprogramming/c-function-strftime.html
	��������� c �ַ���С�� size ���ַ��������ս����ַ�������᷵�ظ��Ƶ� str �е��ַ��������������ս����ַ��������򷵻��㡣
	*/
	strftime(timestr, sizeof timestr, "%H:%M:%S", ltime); //��ӡ Сʱ�����ӣ���
	//��ӡ���ݰ���ʱ����ͳ���
	printf("%s.%.6d len:%d ", timestr, header->ts.tv_usec, header->len);

	//��� IP ���ݰ�ͷ����λ��
	ih = (ip_header*)(pkt_data + 14);// �յ������ݰ���MAC֡��MAC֡��ͷ��Ϊ14���ֽڣ�Ŀ�ĵ�ַ-6�ֽ� --> Դ��ַ-6�ֽ� --> ����-2�ֽڣ���Ȼ�����mac֡�����ݲ��֣�����ʼip���ݱ���
	ip_len = (ih->ver_ihl & 0xf) * 4; //�õ�ip���ײ�����*4����Ϊ�ײ����ȵĵ�λ��4�ֽڣ� ���յõ�ip���ݱ����ֽ�Ϊ��λ���ײ��ĳ���
	ich = (icmp_header*)((u_char*)ih + ip_len); // ���icmp�ײ���λ��
	 //�������ֽ�����ת���������ֽ�����

	//��ӡ IP ��ַ�� tcp �˿�
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

//�����д����־�ļ�
void write_log(char* timestr, const struct pcap_pkthdr* header, MAC_header* mah, ip_header* ih, icmp_header*ich) {
	FILE* fp;
	fp = fopen("Receive.log", "a");//�ڵ�ǰ�����Ŀ¼�´�����׷�����ڶ�д�Ŀյ���־�ļ���Receive.log
	fprintf(fp, "%s.%.6d len:%d \n###MAC�ײ�###\nĿ��MAC��ַ��%X:%X:%X:%X:%X:%X \n ԴMAC��ַ�� %X:%X:%X:%X:%X:%X \n���ͣ�%X\n",
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
	u_char version = ih->ver_ihl >> 4;//��ְ汾
	u_char head_len = ih->ver_ihl & 0x0f;//����ײ�����
	u_char flag = ih->flags_fo >> 13;//��־
	u_short mov_to = ih->flags_fo & 0x1fff;//Ƭƫ��
	char* s;
	//���Э��

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

	fprintf(fp, "###IP���ݱ��ײ�###\n�汾��%d\n�ײ����ȣ�%d\n�ܳ��ȣ�%d�ֽ�\n��ʶ��%x\n��־��%x\nƬƫ�ƣ�%0x(*8�ֽ�)\n����ʱ�䣺%d\nЭ�飺%s\n�ײ�У��ͣ�%x\nԴ��ַ��%d.%d.%d.%d\nĿ�ĵ�ַ��%d.%d.%d.%d\n",
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
		s = new char[strlen("��ѯ����----Ӧ��") + 1];
		strcpy(s, "��ѯ����----Ӧ��");
		break;
	case 3:
		s = new char[strlen("����汨��----Ŀ�Ĳ��ɴ�") + 1];
		strcpy(s, "����汨��----Ŀ�Ĳ��ɴ�");
		break;
	case 4:
		s = new char[strlen("���Ʊ���----Դ����") + 1];
		strcpy(s, "���Ʊ���----Դ����");
		break;
	case 5:
		s = new char[strlen("���Ʊ���----·���ض���") + 1];
		strcpy(s, "���Ʊ���----·���ض���");
		break;
	case 8:
		s = new char[strlen("��ѯ����----��������") + 1];
		strcpy(s, "��ѯ����----��������");
		break;
	case 10:
		s = new char[strlen("��ѯ����----·��������") + 1];
		strcpy(s, "��ѯ����----·��������");
		break;
	case 11:
		s = new char[strlen("����汨��----��ʱ") + 1];
		strcpy(s, "����汨��----��ʱ");
		break;
	case 12:
		s = new char[strlen("����汨��----��������") + 1];
		strcpy(s, "����汨��----��������");
		break;
	case 13:
		s = new char[strlen("��ѯ����----ʱ�������") + 1];
		strcpy(s, "��ѯ����----ʱ�������");
		break;
	case 14:
		s = new char[strlen("��ѯ����----ʱ���Ӧ��") + 1];
		strcpy(s, "��ѯ����----ʱ���Ӧ��");
		break;
	case 17:
		s = new char[strlen("��ѯ����----��������") + 1];
		strcpy(s, "��ѯ����----��������");
		break;
	case 18:
		s = new char[strlen("��ѯ����----����Ӧ��") + 1];
		strcpy(s, "��ѯ����----����Ӧ��");
		break;
	case 19:
		s = new char[strlen("����汨��----ͨ��") + 1];
		strcpy(s, "����汨��----ͨ��");
		break;
	default:
		s = new char[strlen("Unkown") + 1];
		strcpy(s, "Unkown");
		break;
	}
	fprintf(fp, "###ICMP�ײ�###\n���ͣ�%s\n���룺%x\nУ��ͣ�%x\n",
		s,
		ich->code,
		ich->checksum
	);
	fprintf(fp, "=================================================================\n");
	fclose(fp);
}