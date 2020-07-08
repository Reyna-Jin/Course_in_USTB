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

#define PORT 8001 ///端口号 
#define BUFFER_SIZE 1024  //缓冲区的大小
#define FILE_NAME_MAX_SIZE 512  //文件名的长度
#define broRevPort 7000
#define OnLine '1'//告知他人当前机子上线了
#define UnLine '2'//告知他人当前机子下线了
#define SentFile '3'//发送文件

vector<char*> onlineList;//在线好友列表


//向好友列表添加IP
void add(char* addr) {
	bool f = false;
	//查看当前要加入的IP地址是否已经存在
	//此处不能用自带的count函数！这样匹配的是char*的地址
	for (int i = 0; i < onlineList.size(); i++) {
		if (*addr == *onlineList[i]) {
			f = true;
			break;
		}
	}
	if (!f) {
		onlineList.push_back(addr);
		printf("添加成功!\n");
	}
	else {
		printf("当前好友列表中已经存在该IP了\n");
	}
	return;
}
//删除列表中的下线好友
void del(char* addr) {
	bool f = false;
	//查看当前要加入的IP地址是否已经存在
	//此处不能用自带的count函数！这样匹配的是char*的地址
	int i = 0;
	for (; i < onlineList.size(); i++) {
		if (*addr == *onlineList[i]) {
			f = true;
			break;
		}
	}
	if (f) {
		onlineList.erase(onlineList.begin() + i);
		printf("删除成功！\n");
	}
	else {
		printf("当前好友列表中没有该IP\n");
	}
	return;
}
//向发送udp的客户端发起TCP请求
void sendOnLine(char * addr) {
	SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == c_Socket)
	{
		printf("#               创建sock失败！                #\n");
		return;
	}

	// 指定服务端的地址  
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(addr);
	server_addr.sin_port = htons(PORT);

	// 建立连接
	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("#          无法连接到服务器！          #\n");
		return;
	}
	printf("#          成功连接到服务器！          #\n");
	char bufferSendLabel[BUFFER_SIZE] = { 0 };
	memset(bufferSendLabel, 0, BUFFER_SIZE);
	bufferSendLabel[0] = OnLine;
	bufferSendLabel[1] = '\0';
	if (send(c_Socket, bufferSendLabel, BUFFER_SIZE, 0) < 0)
	{
		printf("#                 发送上线回应失败！          #\n");
		return;
	}

	closesocket(c_Socket);
	return;
}
//向用户发送下线通知
void sendUnLine(char* sIP) {
	SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == c_Socket)
	{
		printf("#               创建sock失败！                #\n");
		return;
	}
	for (int i = 0; i < onlineList.size(); i++) {
		// 指定服务端的地址  
		sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.S_un.S_addr = inet_addr(onlineList[i]);
		server_addr.sin_port = htons(PORT);

		// 建立连接
		if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
		{
			printf("#          无法连接到服务器！          #\n");
			return;
		}
		printf("#          成功连接到服务器！          #\n");
		//发送选项
		char bufferSendLabel[BUFFER_SIZE] = { 0 };
		memset(bufferSendLabel, 0, BUFFER_SIZE);
		bufferSendLabel[0] = UnLine;
		bufferSendLabel[1] = '\0';
		if (send(c_Socket, bufferSendLabel, BUFFER_SIZE, 0) < 0)
		{
			printf("#                 发送下线label失败！          #\n");
			return;
		}
	}
	

	closesocket(c_Socket);
	return;
}
//发送文件
void sentfile(char*deIP, char* file_name) {

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
	server_addr.sin_addr.S_un.S_addr = inet_addr(deIP);
	server_addr.sin_port = htons(PORT);

	// 建立连接
	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("#          无法连接到服务器！          #\n");
		return;
	}
	printf("#          成功连接到服务器！          #\n");
	//发送选项
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	buffer[0] = SentFile;
	buffer[1] = '\0';
	if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		printf("#                 发送label失败！          #\n");
		return;
	}

	
	
	
	// 打开文件，准备读取文件  
	FILE* fp = fopen(file_name, "rb"); //windows下是"rb",表示打开一个只写的二进制文件  
	if (NULL == fp)
	{
		printf("#           文件名为 %s的文件未找到！           #\n", file_name);
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
		// 向服务器发送文件名
		memset(buffer, 0, BUFFER_SIZE);
		strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
		if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
		{
			printf("#               发送文件名失败！          #\n");
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
		printf("#      发送文件%s 成功!              #\n", file_name);
	}
	closesocket(c_Socket);
	return;
}

void serviceTCP(char* IP) {

	// 声明并初始化一个服务端(本地)的地址结构  
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	//server_addr.sin_addr.S_un.S_addr = IP;
	server_addr.sin_port = htons(PORT);


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

	sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	while (1)
	{
		//printf("正在监听来自客户端的消息...\n");
		memset(&client_addr, 0, sizeof(sockaddr_in));
		

		SOCKET m_New_Socket = accept(m_Socket, (sockaddr*)&client_addr, &client_addr_len);
		if (SOCKET_ERROR == m_New_Socket)
		{
			printf("Server Accept Failed: %d\n", WSAGetLastError());
			break;
		}

		char bufferTCPServer[BUFFER_SIZE] = { 0 };
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
		memset(bufferTCPServer, 0, BUFFER_SIZE);
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
		if (recv(m_New_Socket, bufferTCPServer, BUFFER_SIZE, 0) < 0)
		{
			printf("服务器接收客户端的label失败!\n");
			break;
		}
		int f = 0;
		switch (bufferTCPServer[0])
		{
		case OnLine://上线消息
			f = 1;
			break;
		case UnLine://下线消息
			f = 2;
			break;
		case SentFile://发送文件消息
			f = 3;
			break;
		default:
			printf("无用的选项\n");
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
				printf("客户端上没找到文件!\n");
			}
			else {
				memset(bufferTCPServer, 0, BUFFER_SIZE);
				if (recv(m_New_Socket, bufferTCPServer, BUFFER_SIZE, 0) < 0)
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
					
				strncpy(file_name, bufferTCPServer, strlen(bufferTCPServer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(bufferTCPServer));
				printf("%s\n", file_name);
				printf("开始接收客户端发送过来的文件%s\n", file_name);
				FILE* fp = fopen(file_name, "wb");
				if (NULL == fp)
				{
					printf("无法写入文件名为 %s的文件\n", file_name);
				}
				else
				{
					memset(bufferTCPServer, 0, BUFFER_SIZE);
					int length = 0;

					while ((length = recv(m_New_Socket, bufferTCPServer, BUFFER_SIZE, 0)) > 0)
					{
						if (fwrite(bufferTCPServer, sizeof(char), length, fp) < length)
						{
							printf("文件名为%s的文件写入失败！\n", file_name);
							break;
						}
						memset(bufferTCPServer, 0, BUFFER_SIZE);
					}

					fclose(fp);
					printf("成功接收来自客户端的%s文件!\n", file_name);
				}
				closesocket(m_New_Socket);
			}

		}

	}

	closesocket(m_Socket);
	return ;
}
void initNetwork() {
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
		exit(1);
		system("pause");
		return;
	}

}
void serverudp(char* IP) {
	initNetwork();
	//创建一个udp socket
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
		// 用来保存客户端的网络信息。
		memset(&client_addr, 0, sizeof(sockaddr_in));

		// 接收客户端发来的信息,server阻塞在这里，等待client发来报文。

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
			printf("UDP receive Failed！\n");
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
		printf("无法把套接字设为广播类型\n");
		return;
	}

	sockaddr_in brocast;
	char buf[BUFFER_SIZE] = { 0 };
	brocast.sin_family = AF_INET;
	brocast.sin_addr.s_addr = INADDR_BROADCAST;//广播地址
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
//刷新用户
void refreshUser(char* sIP) {
	clientudp(sIP, OnLine);
	//Sleep(1000);
}
//显示当前在线用户
void showOnLineUser(char* sIP) {
	//refreshUser(sIP);
	//Sleep(100);
	printf("当前在线好友：\n");
	printf("序号			IP地址\n");
	for (int i = 0; i < onlineList.size(); i++) {
		printf("%d			%s\n", i + 1, onlineList[i]);
	}
}

int main()
{
	initNetwork();
	printf("#-----------------欢迎来到《局域网文件传送程序》-------------#\n");
	printf("#-----------------请指定你的IP地址----------------------------#\n");
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
		printf("#------------------请选择你要进行的操作------------------#\n");
		printf("#------------------1.刷新好友列表【1】-------------------#\n");
		printf("#------------------2.显示好友列表【2】-------------------#\n");
		printf("#------------------3.添加好友【3】-----------------------#\n");
		printf("#------------------4.删除好友【4】-----------------------#\n");
		printf("#------------------5.发送文件【5】-----------------------#\n");
		printf("#------------------6.退出程序【6】-----------------------#\n");
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
			printf("#------------------请输入要添加的好友IP-----------------------#\n");
			memset(newIP, 0, 20);
			cin >> newIP;
			add(newIP);
			break;
		case 4:
			printf("#------------------请输入要删除的好友IP-----------------------#\n");
			memset(newIP, 0, 20);
			cin >> newIP;
			del(newIP);
			break;
		case 5:
			printf("#                   请输入要传送的文件名                 #\n");
			char file_name[FILE_NAME_MAX_SIZE + 1];
			memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
			scanf("%s", &file_name);
			printf("#------------------请输入要发送文件的好友序号（以0结束）------#\n");
			int k;
			cin >> k;
			
			while (k != 0) {
				if(k<=onlineList.size())
					SendList.push_back(k);
				else {
					printf("没有这个序号\n");
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
			printf("输入错误请重新输入\n");
			break;
		}

	}
	printf("#--------您已退出程序，感谢您的使用，再见！---------#\n");
	
	system("pause");
	return 0;
}