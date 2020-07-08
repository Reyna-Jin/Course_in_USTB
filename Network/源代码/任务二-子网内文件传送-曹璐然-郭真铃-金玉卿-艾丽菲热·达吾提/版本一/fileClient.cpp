#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <WinSock2.h>  
#include<iostream>
using namespace std;
#pragma warning (disable: 4996)
#define PORT 8001 ///端口号 
#define BUFFER_SIZE 1024  //缓冲区的大小
#define FILE_NAME_MAX_SIZE 512  //文件名的长度
#pragma comment(lib, "WS2_32") 
void sentfile() {
	printf("#            请输出需要连接的服务器的IP地址            #\n");
	char SERVER_IPADRESS[20];
	cin >> SERVER_IPADRESS;
	// 初始化socket dll  
	WSADATA wsaData;
	// MAKEWORD
	// 原型：#define MAKEWORD(a, b) ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
	// 作用：将两个byte型的a和b合并为一个word型，高8为是b，低8位是a；
	// 返回值：一个无符号的16位整形
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("#               初始化sock dll失败!             #\n");
		//exit(1);
		return;
	}

	// 创建socket  
	// AF_INET: IPv4 网络协议的套接字类型;
	// SOCK_STREAM:提供面向连接的稳定数据传输，即TCP协议;
	SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == c_Socket)
	{
		printf("#               创建sock失败！                #\n");
		//system("pause");
		//exit(1);
		return;
	}

	// 指定服务端的地址  
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IPADRESS);
	server_addr.sin_port = htons(PORT);

	// 建立连接
	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("#          无法连接到服务器！          #\n");
		//system("pause");
		//exit(1);
		return;
	}
	printf("#          成功连接到服务器！          #\n");
	//发送选项
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	buffer[0] = '1';
	buffer[1] = '\0';
	if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		printf("#                 发送选项失败！          #\n");
		//system("pause");
		//exit(1);
		return;
	}

	printf("#                   请输入要传送的文件名                 #\n");
	char file_name[FILE_NAME_MAX_SIZE + 1];
	memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
	scanf("%s", &file_name);
	
	
	// 打开文件，准备读取文件  
	FILE* fp = fopen(file_name, "rb"); //windows下是"rb",表示打开一个只写的二进制文件  
	if (NULL == fp)
	{
		printf("#           文件名为 %s的文件未找到！           #\n", file_name);
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

		// 向服务器发送文件名
		memset(buffer, 0, BUFFER_SIZE);
		strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
		if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
		{
			printf("#               发送文件名失败！          #\n");
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
				printf("#        发送文件%s失败！           #\n", file_name);
				break;
			}
			memset(buffer, 0, BUFFER_SIZE);
		}

		fclose(fp);
		printf("#           发送文件%s 成功!              #\n", file_name);
	}
	closesocket(c_Socket);
	return;
}
void revfile() {
	printf("#                    请输出需要连接的服务器的IP地址                #\n");
	char SERVER_IPADRESS[20];
	cin >> SERVER_IPADRESS;
	// 初始化socket dll  
	WSADATA wsaData;
	// MAKEWORD
	// 原型：#define MAKEWORD(a, b) ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
	// 作用：将两个byte型的a和b合并为一个word型，高8为是b，低8位是a；
	// 返回值：一个无符号的16位整形
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("#               初始化sock失败!             #\n");
		//exit(1);
		return;
	}

	// 创建socket  
	// AF_INET: IPv4 网络协议的套接字类型;
	// SOCK_STREAM:提供面向连接的稳定数据传输，即TCP协议;
	SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == c_Socket)
	{
		printf("#               创建sock失败！                #\n");
		//system("pause");
		//exit(1);
		return;
	}

	// 指定服务端的地址  
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IPADRESS);
	server_addr.sin_port = htons(PORT);

	// 建立连接
	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("#          无法连接到服务器！          #\n");
		//system("pause");
		//exit(1);
		return;
	}
	printf("#          成功连接到服务器！          #\n");
	//发送选项
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	buffer[0] = '2';
	buffer[1] = '\0';
	if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		printf("#                 发送选项失败！          #\n");
		//system("pause");
		//exit(1);
		return;
	}

	// 输入文件名  
	char file_name[FILE_NAME_MAX_SIZE + 1];
	memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
	printf("#           请输入要接收的文件名             #\n ");
	scanf("%s", &file_name);
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
	// 向服务器发送文件名  
	if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		printf("#               发送文件名失败！          #\n");
		//system("pause");
		//exit(1);
		return;
	}

	memset(buffer, 0, BUFFER_SIZE);
	recv(c_Socket, buffer, BUFFER_SIZE, 0);
	if (buffer[0] == '0') {
		printf("#             服务器上没有文件名为 %s的文件！            #\n", file_name);
		return;
	}
	// 打开文件，准备写入  
	FILE* fp = fopen(file_name, "wb"); //windows下是"wb",表示打开一个只写的二进制文件  
	if (NULL == fp)
	{
		printf("#              无法写入文件名为 %s的文件！            #\n", file_name);
		//system("pause");
		//exit(1);
		return;
	}
	else
	{

		

		// memset函数：
		// 原型：memset(void *s,int ch,size_t n); 
		// 说明：将s所指向的某一块内存中的后n个字节的内容全部设置为ch指定的ASCII值
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;
		while ((length = recv(c_Socket, buffer, BUFFER_SIZE, 0)) > 0)
		{
			if (fwrite(buffer, sizeof(char), length, fp) < length)
			{
				printf("#           文件名为%s的文件写入失败！          #\n", file_name);
				break;
			}
			memset(buffer, 0, BUFFER_SIZE);
		}

		printf("#           成功接收来自服务器的%s文件!               #\n", file_name);
	}

	fclose(fp);
	closesocket(c_Socket);

	//释放winsock库  
	WSACleanup();

	//system("pause");
}
int main()
{
	printf("#-----------------欢迎来到《局域网文件传送程序》-------------#\n");
	bool f = true;
	while (f) {
		printf("#------------------请选择你要进行的操作------------------#\n");
		printf("#------------------1.传输文件【1】-----------------------#\n");
		printf("#------------------2.接收文件【2】-----------------------#\n");
		printf("#------------------3.退出程序【3】-----------------------#\n");
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
			printf("输入错误请重新输入\n");
			break;
		}

	}
	printf("#--------您已退出程序，感谢您的使用，再见！---------#\n");
	system("pause");
	return 0;
}











