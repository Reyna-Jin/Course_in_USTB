#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <WinSock2.h>  
#include<iostream>
using namespace std;
#pragma warning (disable: 4996)
#define PORT 8001 ///�˿ں� 
#define BUFFER_SIZE 1024  //�������Ĵ�С
#define FILE_NAME_MAX_SIZE 512  //�ļ����ĳ���
#pragma comment(lib, "WS2_32") 
void sentfile() {
	printf("#            �������Ҫ���ӵķ�������IP��ַ            #\n");
	char SERVER_IPADRESS[20];
	cin >> SERVER_IPADRESS;
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
		//exit(1);
		return;
	}

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
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IPADRESS);
	server_addr.sin_port = htons(PORT);

	// ��������
	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("#          �޷����ӵ���������          #\n");
		//system("pause");
		//exit(1);
		return;
	}
	printf("#          �ɹ����ӵ���������          #\n");
	//����ѡ��
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	buffer[0] = '1';
	buffer[1] = '\0';
	if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		printf("#                 ����ѡ��ʧ�ܣ�          #\n");
		//system("pause");
		//exit(1);
		return;
	}

	printf("#                   ������Ҫ���͵��ļ���                 #\n");
	char file_name[FILE_NAME_MAX_SIZE + 1];
	memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
	scanf("%s", &file_name);
	
	
	// ���ļ���׼����ȡ�ļ�  
	FILE* fp = fopen(file_name, "rb"); //windows����"rb",��ʾ��һ��ֻд�Ķ������ļ�  
	if (NULL == fp)
	{
		printf("#           �ļ���Ϊ %s���ļ�δ�ҵ���           #\n", file_name);
		memset(buffer, 0, BUFFER_SIZE);
		buffer[0] = '0';
		send(c_Socket, buffer, BUFFER_SIZE, 0);

		//system("pause");
		//exit(1);
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
			//system("pause");
			//exit(1);
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
		printf("#           �����ļ�%s �ɹ�!              #\n", file_name);
	}
	closesocket(c_Socket);
	return;
}
void revfile() {
	printf("#                    �������Ҫ���ӵķ�������IP��ַ                #\n");
	char SERVER_IPADRESS[20];
	cin >> SERVER_IPADRESS;
	// ��ʼ��socket dll  
	WSADATA wsaData;
	// MAKEWORD
	// ԭ�ͣ�#define MAKEWORD(a, b) ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
	// ���ã�������byte�͵�a��b�ϲ�Ϊһ��word�ͣ���8Ϊ��b����8λ��a��
	// ����ֵ��һ���޷��ŵ�16λ����
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("#               ��ʼ��sockʧ��!             #\n");
		//exit(1);
		return;
	}

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
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IPADRESS);
	server_addr.sin_port = htons(PORT);

	// ��������
	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("#          �޷����ӵ���������          #\n");
		//system("pause");
		//exit(1);
		return;
	}
	printf("#          �ɹ����ӵ���������          #\n");
	//����ѡ��
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	buffer[0] = '2';
	buffer[1] = '\0';
	if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		printf("#                 ����ѡ��ʧ�ܣ�          #\n");
		//system("pause");
		//exit(1);
		return;
	}

	// �����ļ���  
	char file_name[FILE_NAME_MAX_SIZE + 1];
	memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
	printf("#           ������Ҫ���յ��ļ���             #\n ");
	scanf("%s", &file_name);
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
	// ������������ļ���  
	if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		printf("#               �����ļ���ʧ�ܣ�          #\n");
		//system("pause");
		//exit(1);
		return;
	}

	memset(buffer, 0, BUFFER_SIZE);
	recv(c_Socket, buffer, BUFFER_SIZE, 0);
	if (buffer[0] == '0') {
		printf("#             ��������û���ļ���Ϊ %s���ļ���            #\n", file_name);
		return;
	}
	// ���ļ���׼��д��  
	FILE* fp = fopen(file_name, "wb"); //windows����"wb",��ʾ��һ��ֻд�Ķ������ļ�  
	if (NULL == fp)
	{
		printf("#              �޷�д���ļ���Ϊ %s���ļ���            #\n", file_name);
		//system("pause");
		//exit(1);
		return;
	}
	else
	{

		

		// memset������
		// ԭ�ͣ�memset(void *s,int ch,size_t n); 
		// ˵������s��ָ���ĳһ���ڴ��еĺ�n���ֽڵ�����ȫ������Ϊchָ����ASCIIֵ
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;
		while ((length = recv(c_Socket, buffer, BUFFER_SIZE, 0)) > 0)
		{
			if (fwrite(buffer, sizeof(char), length, fp) < length)
			{
				printf("#           �ļ���Ϊ%s���ļ�д��ʧ�ܣ�          #\n", file_name);
				break;
			}
			memset(buffer, 0, BUFFER_SIZE);
		}

		printf("#           �ɹ��������Է�������%s�ļ�!               #\n", file_name);
	}

	fclose(fp);
	closesocket(c_Socket);

	//�ͷ�winsock��  
	WSACleanup();

	//system("pause");
}
int main()
{
	printf("#-----------------��ӭ�������������ļ����ͳ���-------------#\n");
	bool f = true;
	while (f) {
		printf("#------------------��ѡ����Ҫ���еĲ���------------------#\n");
		printf("#------------------1.�����ļ���1��-----------------------#\n");
		printf("#------------------2.�����ļ���2��-----------------------#\n");
		printf("#------------------3.�˳�����3��-----------------------#\n");
		int n;
		cin >> n;
		switch (n)
		{
		case 1:
			sentfile();
			break;
		case 2:
			revfile();
			break;
		case 3:
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











