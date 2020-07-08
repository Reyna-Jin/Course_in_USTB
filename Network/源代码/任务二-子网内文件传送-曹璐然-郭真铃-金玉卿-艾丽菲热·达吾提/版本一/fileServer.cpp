#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <WinSock2.h>
#include<iostream>
using namespace std;

#pragma warning (disable: 4996)
#define PORT 8001  
#define BUFFER_SIZE 1024  
#define FILE_NAME_MAX_SIZE 512  
#pragma comment(lib, "WS2_32")  

int main()
{
	char SERVER_IPADRESSADRESS[20];
	printf("��ָ������������ip��ַ\n");
	cin >> SERVER_IPADRESSADRESS;
	// ��������ʼ��һ�������(����)�ĵ�ַ�ṹ  
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	// ��ʼ��socket dll  
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("#               ��ʼ��sock dllʧ��!             #\n");
		system("pause");
		exit(1);
	}

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


	while (1)
	{
		printf("���ڼ������Կͻ��˵���Ϣ...\n");

		sockaddr_in client_addr;
		int client_addr_len = sizeof(client_addr);

		SOCKET m_New_Socket = accept(m_Socket, (sockaddr*)&client_addr, &client_addr_len);
		if (SOCKET_ERROR == m_New_Socket)
		{
			printf("Server Accept Failed: %d\n", WSAGetLastError());
			break;
		}

		char buffer[BUFFER_SIZE];
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
		memset(buffer, 0, BUFFER_SIZE);
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
		if (recv(m_New_Socket, buffer, BUFFER_SIZE, 0) < 0)
		{
			printf("���������տͻ��˵�optionʧ��!\n");
			break;
		}
		int f = 0;
		switch (buffer[0])
		{
		case '1':
			f = 1;
			break;
		case '2':
			f = 2;
			break;
		default:
			printf("���õ�ѡ��\n");
			break;
		}

		

		if (f == 1) {
			memset(buffer, 0, BUFFER_SIZE);
			recv(m_New_Socket, buffer, BUFFER_SIZE, 0);
			if (buffer[0] == '0') {
				printf("�ͻ�����û�ҵ��ļ�!\n");
			}
			else {
				memset(buffer, 0, BUFFER_SIZE);
				if (recv(m_New_Socket, buffer, BUFFER_SIZE, 0) < 0)
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
				strncpy(file_name, buffer, strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
				printf("%s\n", file_name);

				printf("��ʼ���տͻ��˷��͹������ļ�%s\n", file_name);
				FILE* fp = fopen(file_name, "wb");
				if (NULL == fp)
				{
					printf("�޷�д���ļ���Ϊ %s���ļ�\n", file_name);
				}
				else
				{
					memset(buffer, 0, BUFFER_SIZE);
					int length = 0;

					while ((length = recv(m_New_Socket, buffer, BUFFER_SIZE, 0)) > 0)
					{
						if (fwrite(buffer, sizeof(char), length, fp) < length)
						{
							printf("�ļ���Ϊ%s���ļ�д��ʧ�ܣ�\n", file_name);
							break;
						}
						memset(buffer, 0, BUFFER_SIZE);
					}

					fclose(fp);
					printf("�ɹ��������Է�������%s�ļ�!\n", file_name);
				}
				closesocket(m_New_Socket);
			}
			
		}
		else if(f == 2) {
			memset(buffer, 0, BUFFER_SIZE);
			if (recv(m_New_Socket, buffer, BUFFER_SIZE, 0) < 0)
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
			strncpy(file_name, buffer, strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
			printf("%s\n", file_name);

			FILE* fp = fopen(file_name, "rb");  // windows����"rb",��ʾ��һ��ֻ���Ķ������ļ�  
			if (NULL == fp)
			{
				printf("�ļ���Ϊ %s���ļ�δ�ҵ���\n", file_name);
				memset(buffer, 0, BUFFER_SIZE);
				buffer[0] = '0';
				send(m_New_Socket, buffer, BUFFER_SIZE, 0);
			}
			else
			{
				memset(buffer, 0, BUFFER_SIZE);
				buffer[0] = '1';
				send(m_New_Socket, buffer, BUFFER_SIZE, 0);

				printf("��ʼ��ͻ��˷���ָ���ļ�%s\n", file_name);
				memset(buffer, 0, BUFFER_SIZE);
				int length = 0;

				while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
				{
					if (send(m_New_Socket, buffer, length, 0) < 0)
					{
						printf("�����ļ�%sʧ�ܣ�\n", file_name);
						break;
					}
					memset(buffer, 0, BUFFER_SIZE);
				}

				fclose(fp);
				printf("�����ļ�%s �ɹ�!\n", file_name);
			}
			closesocket(m_New_Socket);
		}
		
	}

	closesocket(m_Socket);
	// �ͷ�winsock��  
	WSACleanup();
	system("pause");
	return 0;
}
