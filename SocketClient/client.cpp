#define WIN32_LEAN_AND_MEAM
#include<iostream>
#include<WinSock2.h>//包含Socket的头文件
#include<windows.h>//包含windows的头文件
#pragma comment(lib,"ws2_32.lib")//加入静态链接库


int main()
{
	//启动Windows socket2.x环境
	WORD ver = MAKEWORD(2, 2);//()中是版本号
	WSADATA dat;
	WSAStartup(ver, &dat);//启动win_Socket网络环境
	//1 创建套接字
	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) {
		std::cout << "error,建立socket失败" << std::endl;
	}
	else {
		std::cout << "建立socket成功" << std::endl;
	}
	//2 准备地址结构
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//3 请求连接
	int ret = connect(sockfd, (const sockaddr*)&sin, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR) {
		std::cout << "error,连接服务器失败" << std::endl;
	}
	else {
		std::cout << "连接服务器成功" << std::endl;
	}
	
	while (true) {
		//4 发送请求
		//输入请求
		char cmdBuf[128] = {};
		std::cin >> cmdBuf;
		if (strcmp(cmdBuf, "exit") == 0) {
			std::cout << "收到退出命令" << std::endl;
			break;
		}
		else {
			send(sockfd, cmdBuf, strlen(cmdBuf) + 1, 0);
		}

		//5 接收响应
		char recvBuf[128] = {};
		int nLen = recv(sockfd, recvBuf, 128, 0);
		if (nLen > 0) {
			std::cout << "接收到数据：" << recvBuf << std::endl;
		}

	}
	//6 关闭套接字
	closesocket(sockfd);
	WSACleanup();//和WSAStartup匹配
	std::cout << "已退出，任务结束" << std::endl;
	getchar();
	return 0;
}