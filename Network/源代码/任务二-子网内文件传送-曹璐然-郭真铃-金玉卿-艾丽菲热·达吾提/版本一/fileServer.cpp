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
	printf("请指定本服务器的ip地址\n");
	cin >> SERVER_IPADRESSADRESS;
	// 声明并初始化一个服务端(本地)的地址结构  
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	// 初始化socket dll  
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("#               初始化sock dll失败!             #\n");
		system("pause");
		exit(1);
	}

	// 创建socket  
	SOCKET m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == m_Socket)
	{
		printf("#               创建sock失败！                #\n");
		system("pause");
		exit(1);
	}

	// 绑定socket和服务端(本地)地址  
	if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("服务器绑定失败: %d\n", WSAGetLastError());
		system("pause");
		exit(1);
	}

	// 监听  
	if (SOCKET_ERROR == listen(m_Socket, 10))
	{
		printf("服务器无法监听: %d\n", WSAGetLastError());
		system("pause");
		exit(1);
	}


	while (1)
	{
		printf("正在监听来自客户端的消息...\n");

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
		void *memset(void *str, int c, size_t n) 复制字符 c（一个无符号字符）到参数 str 所指向的字符串的前 n 个字符。
		参数：
		str -- 指向要填充的内存块。
		c -- 要被设置的值。该值以 int 形式传递，但是函数在填充内存块时是使用该值的无符号字符形式。
		n -- 要被设置为该值的字节数。
		返回值
		该值返回一个指向存储区 str 的指针。
		*/
		//清空缓冲区
		memset(buffer, 0, BUFFER_SIZE);
		/*
		功能： 从本地已经建立连接的数据报或流式套接口接收数据。格式如下：
			int recv( SOCKET s, const char * buf, int len, int flags);
			参数： s：数据接收方（与发送方已建立连接）的套接字描述符；
			buf：用于接收数据的字符缓冲区指针；
			len：用户缓冲区长度，以字节为单位；
			flags：执行此调用的方式，一般设置为 0。
			返回值：执行成功返回从套接字 s 实际读入到 buf 中的数据字节总数；如果连
			接中止，返回 0； 否则返回 SOCKET_ERROR。
		*/
		if (recv(m_New_Socket, buffer, BUFFER_SIZE, 0) < 0)
		{
			printf("服务器接收客户端的option失败!\n");
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
			printf("无用的选项\n");
			break;
		}

		

		if (f == 1) {
			memset(buffer, 0, BUFFER_SIZE);
			recv(m_New_Socket, buffer, BUFFER_SIZE, 0);
			if (buffer[0] == '0') {
				printf("客户端上没找到文件!\n");
			}
			else {
				memset(buffer, 0, BUFFER_SIZE);
				if (recv(m_New_Socket, buffer, BUFFER_SIZE, 0) < 0)
				{
					printf("服务器接收文件名失败!\n");
					break;
				}
				char file_name[FILE_NAME_MAX_SIZE + 1];
				memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
				/*
				char *strncpy(char *dest, const char *src, size_t n)
				把 src 所指向的字符串复制到 dest，最多复制 n 个字符。当 src 的长度小于 n 时，dest 的剩余部分将用空字节填充。
				参数
				dest -- 指向用于存储复制内容的目标数组。
				src -- 要复制的字符串。
				n -- 要从源中复制的字符数。
				返回值
				该函数返回最终复制的字符串。
				*/
				strncpy(file_name, buffer, strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
				printf("%s\n", file_name);

				printf("开始接收客户端发送过来的文件%s\n", file_name);
				FILE* fp = fopen(file_name, "wb");
				if (NULL == fp)
				{
					printf("无法写入文件名为 %s的文件\n", file_name);
				}
				else
				{
					memset(buffer, 0, BUFFER_SIZE);
					int length = 0;

					while ((length = recv(m_New_Socket, buffer, BUFFER_SIZE, 0)) > 0)
					{
						if (fwrite(buffer, sizeof(char), length, fp) < length)
						{
							printf("文件名为%s的文件写入失败！\n", file_name);
							break;
						}
						memset(buffer, 0, BUFFER_SIZE);
					}

					fclose(fp);
					printf("成功接收来自服务器的%s文件!\n", file_name);
				}
				closesocket(m_New_Socket);
			}
			
		}
		else if(f == 2) {
			memset(buffer, 0, BUFFER_SIZE);
			if (recv(m_New_Socket, buffer, BUFFER_SIZE, 0) < 0)
			{
				printf("服务器接收文件名失败!\n");
				break;
			}
			char file_name[FILE_NAME_MAX_SIZE + 1];
			memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
			/*
			char *strncpy(char *dest, const char *src, size_t n)
			把 src 所指向的字符串复制到 dest，最多复制 n 个字符。当 src 的长度小于 n 时，dest 的剩余部分将用空字节填充。
			参数
			dest -- 指向用于存储复制内容的目标数组。
			src -- 要复制的字符串。
			n -- 要从源中复制的字符数。
			返回值
			该函数返回最终复制的字符串。
			*/
			strncpy(file_name, buffer, strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
			printf("%s\n", file_name);

			FILE* fp = fopen(file_name, "rb");  // windows下是"rb",表示打开一个只读的二进制文件  
			if (NULL == fp)
			{
				printf("文件名为 %s的文件未找到！\n", file_name);
				memset(buffer, 0, BUFFER_SIZE);
				buffer[0] = '0';
				send(m_New_Socket, buffer, BUFFER_SIZE, 0);
			}
			else
			{
				memset(buffer, 0, BUFFER_SIZE);
				buffer[0] = '1';
				send(m_New_Socket, buffer, BUFFER_SIZE, 0);

				printf("开始向客户端发送指定文件%s\n", file_name);
				memset(buffer, 0, BUFFER_SIZE);
				int length = 0;

				while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
				{
					if (send(m_New_Socket, buffer, length, 0) < 0)
					{
						printf("发送文件%s失败！\n", file_name);
						break;
					}
					memset(buffer, 0, BUFFER_SIZE);
				}

				fclose(fp);
				printf("发送文件%s 成功!\n", file_name);
			}
			closesocket(m_New_Socket);
		}
		
	}

	closesocket(m_Socket);
	// 释放winsock库  
	WSACleanup();
	system("pause");
	return 0;
}
