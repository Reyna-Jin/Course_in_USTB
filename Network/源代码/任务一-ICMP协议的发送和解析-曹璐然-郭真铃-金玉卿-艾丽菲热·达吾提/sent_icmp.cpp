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
	//���������ֽ����
	if (len % 2 == 0) {
		check_sum += (packet[len - 1] << 8);
	}
	//�����λ����
	check_sum = (check_sum >> 16) + (check_sum & 0xffff);
	check_sum += (check_sum >> 16);
	//ȡ�� ��ֵ
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
	u_char check_num[2];//���У����
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int inum;
	///* ��������в����ĺϷ��� */
	//if (argc != 2)
	//{
	//    printf("usage: %s interface (e.g. 'rpcap://eth0')", argv[0]);
	//    return;
	//}

	///* ������豸 */
	//if ((fp = pcap_open(argv[1],            // �豸��
	//    100,                // Ҫ����Ĳ��� (ֻ����ǰ100���ֽ�)
	//    PCAP_OPENFLAG_PROMISCUOUS,  // ����ģʽ
	//    1000,               // ����ʱʱ��
	//    NULL,               // Զ�̻�����֤
	//    errbuf              // ���󻺳�
	//)) == NULL)
	//{
	//    fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", argv[1]);
	//    return;
	//}
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
		// ����豸�б�
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}
	for (d = alldevs; d; d = d->next)
	{ // ��ӡ�����������б�
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
	scanf_s("%d", &inum); //scanf C���Եı�׼���뺯��
	if (inum < 1 || inum > j) //�ж������Ƿ���ȷ �Ƿ���1-i֮�������
	{
		printf("nInterface number out of range.\n");
		pcap_freealldevs(alldevs); //�ͷ��豸�б�
		return -1;
	}
	//��ת����ѡ�������俨
	for (d = alldevs, j = 0; j < inum - 1; d = d->next, j++);
	if ((fp = pcap_open(d->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf)) == NULL)//���������俨
	{
		fprintf(stderr, "nUnable to open the adapter. %s is not supported by WinPcapn");
		pcap_freealldevs(alldevs); // �ͷ��豸�б�
		return -1;
	}

	/*-------------��װ����--------------------------*/
/* ����MACĿ�ĵ�ַΪ  B4-69-21-81-31-93*/
	packet[6] = 0xFF;
	packet[7] = 0xFF;
	packet[8] = 0xFF;
	packet[9] = 0xFF;
	packet[10] = 0xFF;
	packet[11] = 0xFF;

	/* ����MAC��Ŀ�ĵ�ַΪ 34-42-62-0E-C2-A5
	packet[0] = 0x34;
	packet[1] = 0x42;
	packet[2] = 0x62;
	packet[3] = 0x0E;
	packet[4] = 0xC2;
	packet[5] = 0xA5;*/

	/* ����MACԴ��ַΪ  B4-69-21-81-31-93*/
	packet[6] = 0xB4;
	packet[7] = 0x69;
	packet[8] = 0x21;
	packet[9] = 0x81;
	packet[10] = 0x31;
	packet[11] = 0x93;

	/*ָ���ϲ��õ���IP���ݱ�*/
	packet[12] = 8;
	packet[13] = 0;
	/*����MAC֡ͷ����װ���*/

	/*IP���ݱ��ײ�*/
	packet[14] = 0x45;//�汾��4 ���ȣ�5
	packet[15] = 0x00;//���ַ��� ����
	packet[16] = 0x00;//�ܳ��ȣ�86�ֽ� ��0x0056
	packet[17] = 0x56;
	packet[18] = 0x3f;//��ʶ��0x3ff2
	packet[19] = 0xf2;
	packet[20] = 0x00;//��־��0 �������ݱ�Ƭ�����һ��
	packet[21] = 0x00;//Ƭƫ�ƣ�0
	packet[22] = 0x10;//����ʱ��TTL
	packet[23] = 0x01;//Э�飺ICMP 1
	packet[24] = 0x00;//�ײ�����ͣ�����Ϊ0
	packet[25] = 0x00;
	//Դ��ַ IP��ַ 192.168.0.100
	packet[26] = 192;
	packet[27] = 168;
	packet[28] = 0;
	packet[29] = 100;
	//Ŀ�ĵ�ַ  192.168.0.100
	//packet[30] = 192;
	//packet[31] = 168;
	//packet[32] = 0;
	//packet[33] = 100;
	// ����Ŀ��ip��ַ
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
	//�����ײ�У��ͣ�IP���ݱ���
	check(packet, 20, check_num);
	packet[24] = check_num[0];
	packet[25] = check_num[1];
	/*ICMP�ײ�*/
	packet[34] = 0x08;//���ͣ�ѯ�ʱ��� ��������
	packet[35] = 0x00;//����
	packet[36] = 0; //У���
	packet[37] = 0;
	packet[38] = 0; //��ʶ
	packet[39] = 1;
	packet[40] = 0;//���к�
	packet[41] = 0;
	/*���ʣ�µ�����*/
	for (i = 42; i < packet_size; i++)
	{
		packet[i] = i % 256;
	}
	
	/*����ICMP��У��� Ҫ������������*/
	check(packet, packet_size - 34, check_num);
	packet[36] = check_num[0];
	packet[37] = check_num[1];


	/* �������ݰ� */
	if (pcap_sendpacket(fp, packet, packet_size) != 0)
	{
		fprintf(stderr, "\nError sending the packet: \n", pcap_geterr(fp));
		return 0;
	}
	printf("Success��");
	return 0;
}