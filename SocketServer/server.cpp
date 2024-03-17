#define WIN32_LEAN_AND_MEAM
#include<iostream>
#include<WinSock2.h>//包含Socket的头文件
#include<windows.h>//包含windows的头文件
#pragma comment(lib,"ws2_32.lib")//加入静态链接库
//用Socket API建立简易TCP服务端

int main() {
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	// 1 建立一个socket
	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) {
		std::cout << "error,建立socket失败" << std::endl;
	}
	else {
		std::cout << "建立socket成功" << std::endl;
	}
    // 2 绑定接受客户端连接的端口 bind 
	sockaddr_in sin = {}; //网络地址结构
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (bind(sockfd, (const sockaddr*)&sin, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		std::cout << "error,绑定用于连接客户端的网络端口失败" << std::endl;
	}
	else
	{
		std::cout << "绑定端口成功" << std::endl;
	}
    // 3 监听网络端口 listen 
	if (listen(sockfd, 1024) == SOCKET_ERROR) {
		std::cout << "error,监听网络端口失败" << std::endl;
	}
	else {
		std::cout << "监听网络端口成功" << std::endl;
	}
	
    // 4 等待接受客户端连接 accept 
	sockaddr_in clientAddr = {};
	int cAddrLen = sizeof(sockaddr_in);
	SOCKET cSock = INVALID_SOCKET;
	cSock = accept(sockfd, (sockaddr*)&clientAddr, &cAddrLen);
	if (cSock == INVALID_SOCKET) {
		std::cout << "error,接收到无效客户端SOCKET" << std::endl;
	}

	// 5 接收请求
	char recvBuf[128] = {};//接收数据缓冲区
	while (true) {
		int nLen = recv(cSock, recvBuf, 128, 0);
		if (nLen <= 0) {
			std::cout << "客户端已退出，任务结束" << std::endl;
			break;
		}
		std::cout << "收到客户端命令：" << recvBuf << std::endl;
		// 6 向客户端发送响应 
		if (strcmp(recvBuf, "getname") == 0) {
			char msgBuf[] = "Ni shuang";
			send(cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if (strcmp(recvBuf, "getage") == 0) {
			char msgBuf[] = "24";
			send(cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else {
			char msgBuf[] = "???";
			send(cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
	}
  
    // 7 关闭socket  closesocket
	closesocket(sockfd);


	WSACleanup();//和WSAStartup匹配
	std::cout << "已退出，任务结束" << std::endl;
	getchar();
	return 0;
}






