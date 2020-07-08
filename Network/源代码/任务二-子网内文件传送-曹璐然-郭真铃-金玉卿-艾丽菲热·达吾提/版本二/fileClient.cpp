#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <WinSock2.h>  
#include<iostream>
#include <thread>
#include<vector>
#include <cstdlib> 
#include<io.h>
using namespace std;
#pragma warning (disable: 4996)
#pragma comment(lib, "WS2_32") 

#define PORT 8001 ///�˿ں� 
#define BUFFER_SIZE 1024  //�������Ĵ�С
#define FILE_NAME_MAX_SIZE 512  //�ļ����ĳ���
#define broRevPort 7000
#define OnLine '1'//��֪���˵�ǰ����������
#define UnLine '2'//��֪���˵�ǰ����������
#define SentFile '3'//�����ļ�

vector<char*> onlineList;//���ߺ����б�


//������б����IP
void add(char* addr) {
	bool f = false;
	//�鿴��ǰҪ�����IP��ַ�Ƿ��Ѿ�����
	//�˴��������Դ���count����������ƥ�����char*�ĵ�ַ
	for (int i = 0; i < onlineList.size(); i++) {
		if (*addr == *onlineList[i]) {
			f = true;
			break;
		}
	}
	if (!f) {
		onlineList.push_back(addr);
		printf("��ӳɹ�!\n");
	}
	else {
		printf("��ǰ�����б����Ѿ����ڸ�IP��\n");
	}
	return;
}
//ɾ���б��е����ߺ���
void del(char* addr) {
	bool f = false;
	//�鿴��ǰҪ�����IP��ַ�Ƿ��Ѿ�����
	//�˴��������Դ���count����������ƥ�����char*�ĵ�ַ
	int i = 0;
	for (; i < onlineList.size(); i++) {
		if (*addr == *onlineList[i]) {
			f = true;
			break;
		}
	}
	if (f) {
		onlineList.erase(onlineList.begin() + i);
		printf("ɾ���ɹ���\n");
	}
	else {
		printf("��ǰ�����б���û�и�IP\n");
	}
	return;
}
//����udp�Ŀͻ��˷���TCP����
void sendOnLine(char * addr) {
	SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == c_Socket)
	{
		printf("#               ����sockʧ�ܣ�                #\n");
		return;
	}

	// ָ������˵ĵ�ַ  
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(addr);
	server_addr.sin_port = htons(PORT);

	// ��������
	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("#          �޷����ӵ���������          #\n");
		return;
	}
	printf("#          �ɹ����ӵ���������          #\n");
	char bufferSendLabel[BUFFER_SIZE] = { 0 };
	memset(bufferSendLabel, 0, BUFFER_SIZE);
	bufferSendLabel[0] = OnLine;
	bufferSendLabel[1] = '\0';
	if (send(c_Socket, bufferSendLabel, BUFFER_SIZE, 0) < 0)
	{
		printf("#                 �������߻�Ӧʧ�ܣ�          #\n");
		return;
	}

	closesocket(c_Socket);
	return;
}
//���û���������֪ͨ
void sendUnLine(char* sIP) {
	SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == c_Socket)
	{
		printf("#               ����sockʧ�ܣ�                #\n");
		return;
	}
	for (int i = 0; i < onlineList.size(); i++) {
		// ָ������˵ĵ�ַ  
		sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.S_un.S_addr = inet_addr(onlineList[i]);
		server_addr.sin_port = htons(PORT);

		// ��������
		if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
		{
			printf("#          �޷����ӵ���������          #\n");
			return;
		}
		printf("#          �ɹ����ӵ���������          #\n");
		//����ѡ��
		char bufferSendLabel[BUFFER_SIZE] = { 0 };
		memset(bufferSendLabel, 0, BUFFER_SIZE);
		bufferSendLabel[0] = UnLine;
		bufferSendLabel[1] = '\0';
		if (send(c_Socket, bufferSendLabel, BUFFER_SIZE, 0) < 0)
		{
			printf("#                 ��������labelʧ�ܣ�          #\n");
			return;
		}
	}
	

	closesocket(c_Socket);
	return;
}
//�����ļ�
void sentfile(char*deIP, char* file_name) {

	// ����socket  
	// AF_INET: IPv4 ����Э����׽�������;
	// SOCK_STREAM:�ṩ�������ӵ��ȶ����ݴ��䣬��TCPЭ��;
	SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == c_Socket)
	{
		printf("#               ����sockʧ�ܣ�                #\n");
		//system("pause");
		//exit(1);
		return;
	}

	// ָ������˵ĵ�ַ  
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(deIP);
	server_addr.sin_port = htons(PORT);

	// ��������
	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("#          �޷����ӵ���������          #\n");
		return;
	}
	printf("#          �ɹ����ӵ���������          #\n");
	//����ѡ��
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	buffer[0] = SentFile;
	buffer[1] = '\0';
	if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		printf("#                 ����labelʧ�ܣ�          #\n");
		return;
	}

	
	
	
	// ���ļ���׼����ȡ�ļ�  
	FILE* fp = fopen(file_name, "rb"); //windows����"rb",��ʾ��һ��ֻд�Ķ������ļ�  
	if (NULL == fp)
	{
		printf("#           �ļ���Ϊ %s���ļ�δ�ҵ���           #\n", file_name);
		memset(buffer, 0, BUFFER_SIZE);
		buffer[0] = '0';
		send(c_Socket, buffer, BUFFER_SIZE, 0);
		return;
	}
	else
	{
		memset(buffer, 0, BUFFER_SIZE);
		buffer[0] = '1';
		send(c_Socket, buffer, BUFFER_SIZE, 0);
		// ������������ļ���
		memset(buffer, 0, BUFFER_SIZE);
		strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
		if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
		{
			printf("#               �����ļ���ʧ�ܣ�          #\n");
			return;
		}

		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;

		while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
		{
			if (send(c_Socket, buffer, length, 0) < 0)
			{
				printf("#        �����ļ�%sʧ�ܣ�           #\n", file_name);
				break;
			}
			memset(buffer, 0, BUFFER_SIZE);
		}

		fclose(fp);
		printf("#      �����ļ�%s �ɹ�!              #\n", file_name);
	}
	closesocket(c_Socket);
	return;
}

void serviceTCP(char* IP) {

	// ��������ʼ��һ�������(����)�ĵ�ַ�ṹ  
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	//server_addr.sin_addr.S_un.S_addr = IP;
	server_addr.sin_port = htons(PORT);


	// ����socket  
	SOCKET m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == m_Socket)
	{
		printf("#               ����sockʧ�ܣ�                #\n");
		system("pause");
		exit(1);
	}

	// ��socket�ͷ����(����)��ַ  
	if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("��������ʧ��: %d\n", WSAGetLastError());
		system("pause");
		exit(1);
	}

	// ����  
	if (SOCKET_ERROR == listen(m_Socket, 10))
	{
		printf("�������޷�����: %d\n", WSAGetLastError());
		system("pause");
		exit(1);
	}

	sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	while (1)
	{
		//printf("���ڼ������Կͻ��˵���Ϣ...\n");
		memset(&client_addr, 0, sizeof(sockaddr_in));
		

		SOCKET m_New_Socket = accept(m_Socket, (sockaddr*)&client_addr, &client_addr_len);
		if (SOCKET_ERROR == m_New_Socket)
		{
			printf("Server Accept Failed: %d\n", WSAGetLastError());
			break;
		}

		char bufferTCPServer[BUFFER_SIZE] = { 0 };
		/*
		void *memset(void *str, int c, size_t n) �����ַ� c��һ���޷����ַ��������� str ��ָ����ַ�����ǰ n ���ַ���
		������
		str -- ָ��Ҫ�����ڴ�顣
		c -- Ҫ�����õ�ֵ����ֵ�� int ��ʽ���ݣ����Ǻ���������ڴ��ʱ��ʹ�ø�ֵ���޷����ַ���ʽ��
		n -- Ҫ������Ϊ��ֵ���ֽ�����
		����ֵ
		��ֵ����һ��ָ��洢�� str ��ָ�롣
		*/
		//��ջ�����
		memset(bufferTCPServer, 0, BUFFER_SIZE);
		/*
		���ܣ� �ӱ����Ѿ��������ӵ����ݱ�����ʽ�׽ӿڽ������ݡ���ʽ���£�
			int recv( SOCKET s, const char * buf, int len, int flags);
			������ s�����ݽ��շ����뷢�ͷ��ѽ������ӣ����׽�����������
			buf�����ڽ������ݵ��ַ�������ָ�룻
			len���û����������ȣ����ֽ�Ϊ��λ��
			flags��ִ�д˵��õķ�ʽ��һ������Ϊ 0��
			����ֵ��ִ�гɹ����ش��׽��� s ʵ�ʶ��뵽 buf �е������ֽ������������
			����ֹ������ 0�� ���򷵻� SOCKET_ERROR��
		*/
		if (recv(m_New_Socket, bufferTCPServer, BUFFER_SIZE, 0) < 0)
		{
			printf("���������տͻ��˵�labelʧ��!\n");
			break;
		}
		int f = 0;
		switch (bufferTCPServer[0])
		{
		case OnLine://������Ϣ
			f = 1;
			break;
		case UnLine://������Ϣ
			f = 2;
			break;
		case SentFile://�����ļ���Ϣ
			f = 3;
			break;
		default:
			printf("���õ�ѡ��\n");
			break;
		}


		if (f == 1) {
			add(inet_ntoa(client_addr.sin_addr));
		}
		else if (f == 2) {
			del(inet_ntoa(client_addr.sin_addr));
		}
		else if (f == 3) {
			memset(bufferTCPServer, 0, BUFFER_SIZE);
			recv(m_New_Socket, bufferTCPServer, BUFFER_SIZE, 0);
			if (bufferTCPServer[0] == '0') {
				printf("�ͻ�����û�ҵ��ļ�!\n");
			}
			else {
				memset(bufferTCPServer, 0, BUFFER_SIZE);
				if (recv(m_New_Socket, bufferTCPServer, BUFFER_SIZE, 0) < 0)
				{
					printf("�����������ļ���ʧ��!\n");
					break;
				}
				char file_name[FILE_NAME_MAX_SIZE + 1];
				memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
				/*
				char *strncpy(char *dest, const char *src, size_t n)
				�� src ��ָ����ַ������Ƶ� dest����ิ�� n ���ַ����� src �ĳ���С�� n ʱ��dest ��ʣ�ಿ�ֽ��ÿ��ֽ���䡣
				����
				dest -- ָ�����ڴ洢�������ݵ�Ŀ�����顣
				src -- Ҫ���Ƶ��ַ�����
				n -- Ҫ��Դ�и��Ƶ��ַ�����
				����ֵ
				�ú����������ո��Ƶ��ַ�����
				*/
					
				strncpy(file_name, bufferTCPServer, strlen(bufferTCPServer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(bufferTCPServer));
				printf("%s\n", file_name);
				printf("��ʼ���տͻ��˷��͹������ļ�%s\n", file_name);
				FILE* fp = fopen(file_name, "wb");
				if (NULL == fp)
				{
					printf("�޷�д���ļ���Ϊ %s���ļ�\n", file_name);
				}
				else
				{
					memset(bufferTCPServer, 0, BUFFER_SIZE);
					int length = 0;

					while ((length = recv(m_New_Socket, bufferTCPServer, BUFFER_SIZE, 0)) > 0)
					{
						if (fwrite(bufferTCPServer, sizeof(char), length, fp) < length)
						{
							printf("�ļ���Ϊ%s���ļ�д��ʧ�ܣ�\n", file_name);
							break;
						}
						memset(bufferTCPServer, 0, BUFFER_SIZE);
					}

					fclose(fp);
					printf("�ɹ��������Կͻ��˵�%s�ļ�!\n", file_name);
				}
				closesocket(m_New_Socket);
			}

		}

	}

	closesocket(m_Socket);
	return ;
}
void initNetwork() {
	// ��ʼ��socket dll  
	WSADATA wsaData;
	// MAKEWORD
	// ԭ�ͣ�#define MAKEWORD(a, b) ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
	// ���ã�������byte�͵�a��b�ϲ�Ϊһ��word�ͣ���8Ϊ��b����8λ��a��
	// ����ֵ��һ���޷��ŵ�16λ����
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("#               ��ʼ��sock dllʧ��!             #\n");
		exit(1);
		system("pause");
		return;
	}

}
void serverudp(char* IP) {
	initNetwork();
	//����һ��udp socket
	SOCKET udp_server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (udp_server_socket == SOCKET_ERROR) {

		std::cout << "Failed to create socket!" << std::endl;

		return;

	}
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(broRevPort);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bool so_broadcast = true;
	int ret = setsockopt(udp_server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&so_broadcast, sizeof(so_broadcast));

	
	//bind(udp_server_socket, (sockaddr*)&server_addr, sizeof(server_addr));
	if (bind(udp_server_socket, (struct sockaddr*) & server_addr, sizeof(struct sockaddr)) == SOCKET_ERROR) {

		printf("bind fail\n");

		return ;

	}
	char bufferUDPServer[BUFFER_SIZE] = { 0 };
	sockaddr_in client_addr;
	int client_addr_len = sizeof(sockaddr);
	while (true) {
		memset(bufferUDPServer, 0, BUFFER_SIZE);
		//printf("udp listening...\n");
		// ��������ͻ��˵�������Ϣ��
		memset(&client_addr, 0, sizeof(sockaddr_in));

		// ���տͻ��˷�������Ϣ,server����������ȴ�client�������ġ�

		ret = recvfrom(udp_server_socket, bufferUDPServer, BUFFER_SIZE, 0, (sockaddr*)&client_addr, &client_addr_len);
		if (ret != SOCKET_ERROR) {
			switch (bufferUDPServer[0])
			{
			case OnLine:
				add(inet_ntoa(client_addr.sin_addr));
				sendOnLine(inet_ntoa(client_addr.sin_addr));
				break;
			default:
				break;
			}
		}
		else {
			printf("UDP receive Failed��\n");
		}
	}



	closesocket(udp_server_socket);
	WSACleanup();
	return;

}
void clientudp(char* IP,char sendLabel) {
	initNetwork();
	SOCKET udp_client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (udp_client_socket == SOCKET_ERROR) {

		std::cout << "Failed to create socket!" << std::endl;

		return ;

	}


	/*
	sockaddr_in client_addr;

	memset(&client_addr, 0, sizeof(sockaddr_in));

	client_addr.sin_family = AF_INET;

	client_addr.sin_port = htons(PORT);

	client_addr.sin_addr.S_un.S_addr = inet_addr(IP);
*/
	bool so_broadcast = true;
	int ret = setsockopt(udp_client_socket, SOL_SOCKET, SO_BROADCAST, (char*)&so_broadcast, sizeof(so_broadcast));
	if (ret < 0)
	{
		printf("�޷����׽�����Ϊ�㲥����\n");
		return;
	}

	sockaddr_in brocast;
	char buf[BUFFER_SIZE] = { 0 };
	brocast.sin_family = AF_INET;
	brocast.sin_addr.s_addr = INADDR_BROADCAST;//�㲥��ַ
	brocast.sin_port = htons(broRevPort);
	memset(buf, 0, BUFFER_SIZE);
	buf[0] = sendLabel;
	ret = sendto(udp_client_socket, buf, strlen(buf), 0, (sockaddr*)&brocast, sizeof(sockaddr));

	if (ret == SOCKET_ERROR) {

		std::cout << "Failed to send!" << std::endl;

		return ;

	}

	closesocket(udp_client_socket);
	WSACleanup();
	return ;

}
//ˢ���û�
void refreshUser(char* sIP) {
	clientudp(sIP, OnLine);
	//Sleep(1000);
}
//��ʾ��ǰ�����û�
void showOnLineUser(char* sIP) {
	//refreshUser(sIP);
	//Sleep(100);
	printf("��ǰ���ߺ��ѣ�\n");
	printf("���			IP��ַ\n");
	for (int i = 0; i < onlineList.size(); i++) {
		printf("%d			%s\n", i + 1, onlineList[i]);
	}
}

int main()
{
	initNetwork();
	printf("#-----------------��ӭ�������������ļ����ͳ���-------------#\n");
	printf("#-----------------��ָ�����IP��ַ----------------------------#\n");
	char sIP[20];
	cin >> sIP;
	thread ts(serviceTCP,sIP);
	ts.detach();
	thread us(serverudp, sIP);
	us.detach();
	Sleep(20);
	refreshUser(sIP);
	
	
	Sleep(1000);
	bool f = true;
	char newIP[20];
	
	while (f) {
		printf("#------------------��ѡ����Ҫ���еĲ���------------------#\n");
		printf("#------------------1.ˢ�º����б�1��-------------------#\n");
		printf("#------------------2.��ʾ�����б�2��-------------------#\n");
		printf("#------------------3.��Ӻ��ѡ�3��-----------------------#\n");
		printf("#------------------4.ɾ�����ѡ�4��-----------------------#\n");
		printf("#------------------5.�����ļ���5��-----------------------#\n");
		printf("#------------------6.�˳�����6��-----------------------#\n");
		vector<int>SendList;
		int n;
		cin >> n;
		switch (n)
		{
		case 1:
			refreshUser(sIP);
			break;
		case 2:
			showOnLineUser(sIP);
			break;
		case 3:
			printf("#------------------������Ҫ��ӵĺ���IP-----------------------#\n");
			memset(newIP, 0, 20);
			cin >> newIP;
			add(newIP);
			break;
		case 4:
			printf("#------------------������Ҫɾ���ĺ���IP-----------------------#\n");
			memset(newIP, 0, 20);
			cin >> newIP;
			del(newIP);
			break;
		case 5:
			printf("#                   ������Ҫ���͵��ļ���                 #\n");
			char file_name[FILE_NAME_MAX_SIZE + 1];
			memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
			scanf("%s", &file_name);
			printf("#------------------������Ҫ�����ļ��ĺ�����ţ���0������------#\n");
			int k;
			cin >> k;
			
			while (k != 0) {
				if(k<=onlineList.size())
					SendList.push_back(k);
				else {
					printf("û��������\n");
				}
				cin >> k;
			}
			for (int i = 0; i < SendList.size(); i++) {
				char* deIP = onlineList[SendList[i] - 1];
				sentfile(deIP, file_name);
			}
			
			break;
		case 6:
			sendUnLine(sIP);
			f = false;
			break;
		default:
			printf("�����������������\n");
			break;
		}

	}
	printf("#--------�����˳����򣬸�л����ʹ�ã��ټ���---------#\n");
	
	system("pause");
	return 0;
}