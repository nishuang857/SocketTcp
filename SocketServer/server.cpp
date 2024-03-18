#define WIN32_LEAN_AND_MEAM
#include<iostream>
#include<WinSock2.h>//包含Socket的头文件
#include<windows.h>//包含windows的头文件
#pragma comment(lib,"ws2_32.lib")//加入静态链接库
#include<vector>
//发送的网络消息数据结构化
//struct DataPackage {
//	int age;
//	char name[32];
//};

//网络报文数据格式结构
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
//包头 描述消息包的大小，描述数据的作用
struct DataHeader {
	short _dataLength;
	short _cmd;
};
//包体 存放数据
struct Login :public DataHeader
{
	Login() {
		_dataLength = sizeof(Login);
		_cmd = CMD_LOGIN;
	}
	char _userName[32];
	char _password[32];
};
struct LoginResult :public DataHeader
{
	LoginResult() {
		_dataLength = sizeof(LoginResult);
		_cmd = CMD_LOGIN_RESULT;
		_result = 1;
	}
	int _result;
};
struct Logout :public DataHeader
{
	Logout() {
		_dataLength = sizeof(Logout);
		_cmd = CMD_LOGOUT;
	}
	char _userName[32];
};
struct LogoutResult :public DataHeader
{
	LogoutResult() {
		_dataLength = sizeof(LogoutResult);
		_cmd = CMD_LOGOUT_RESULT;
		_result = 1;
	}
	int _result;
};
struct NewUserJoin :public DataHeader
{
	NewUserJoin(){
		_dataLength = sizeof(NewUserJoin);
		_cmd = CMD_NEW_USER_JOIN;
		sock = 1;
	}
	int sock;
};

std::vector<SOCKET>g_clients;

//处理客服端发送的请求函数
int processor(SOCKET cSock) {
	//接收数据缓冲区
	char szRecv[1024] = {};
	int nLen = recv(cSock, (char*)&szRecv, sizeof(DataHeader), 0);
	//再放到数据头
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		std::cout << "客户端Socket = "<<cSock<<" 已退出，任务结束" << std::endl;
		return -1;
	}

	// 6 向客户端发送响应 
	switch (header->_cmd)
	{
	case CMD_LOGIN: {
		Login login = {};
		recv(cSock, (char*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
		std::cout << "收到客户端Socket = "<<cSock<<" 命令：CMD_LOGIN" << " 数据长度：" << login._dataLength
			<< " 用户名：" << login._userName << " 密码：" << login._password << std::endl;

		LoginResult ret;
		send(cSock, (char*)&ret, sizeof(LoginResult), 0);

	}break;
	case CMD_LOGOUT: {
		Logout logout = {};
		recv(cSock, (char*)&logout + sizeof(DataHeader), sizeof(Logout) - sizeof(DataHeader), 0);
		std::cout << "收到客户端Socket = " << cSock << " 命令：CMD_LOGOUT" << " 数据长度：" << logout._dataLength
			<< " 用户名：" << logout._userName << std::endl;
		LogoutResult ret;
		send(cSock, (char*)&ret, sizeof(LogoutResult), 0);
	}break;
	default:
		header->_cmd = CMD_ERROR;
		header->_dataLength = 0;
		send(cSock, (char*)&header, sizeof(DataHeader), 0);
		break;
	}
}

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
	
   

	// 等待连接和接收请求放入循环
	while (true) {

		//select网络模型三个描述符
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		FD_SET(sockfd, &fdRead);
		FD_SET(sockfd, &fdWrite);
		FD_SET(sockfd, &fdExp);

		for (int n = (int)g_clients.size() - 1; n >= 0; n--) {
			FD_SET(g_clients[n], &fdRead);
		}

		timeval t = { 1,0 }; //t=1s
		//监视socket集合
		int ret = select(sockfd+1, &fdRead, &fdWrite, &fdExp, &t);
		//nfds 是一个整数值，是指fd_set集合中所有描述符(socket)，而不是数量
		//既是所有文件描述符的最大值 在windows中这个参数可以为0
		if (ret < 0) {
			std::cout << "select任务结束" << std::endl;
			break;
		}
		//查询sockfd是否在fdRead集合中
		if (FD_ISSET(sockfd, &fdRead)) {
			FD_CLR(sockfd, &fdRead);
			// 4 等待接受客户端连接 accept 
			sockaddr_in clientAddr = {};
			int cAddrLen = sizeof(sockaddr_in);
			SOCKET cSock = INVALID_SOCKET;
			cSock = accept(sockfd, (sockaddr*)&clientAddr, &cAddrLen);
			if (cSock == INVALID_SOCKET) {
				std::cout << "error,接收到无效客户端SOCKET" << std::endl;
			}
			//向其他客户端发送新客户端加入的消息
			for (int n = (int)g_clients.size() - 1; n >= 0; n--) {
				NewUserJoin userJoin;
				send(g_clients[n], (const char*)&userJoin, sizeof(NewUserJoin), 0);
			}
			//客户端加入g_clients
			g_clients.push_back(cSock);
			std::cout << "新客户端加入：" << "socket=" << (int)cSock << " IP= " << inet_ntoa(clientAddr.sin_addr) << std::endl;
		}
		//调用处理请求函数
		for (int n = 0; n < (int)fdRead.fd_count; n++) {
			if (processor(fdRead.fd_array[n]) == -1) {
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end()) {
					g_clients.erase(iter);
				}
			}
		}

		//其他业务

		
	}
  
	for (int n = (int)g_clients.size() - 1; n >= 0; n--) {
		closesocket(g_clients[n]);
	}

    // 7 关闭socket  closesocket
	closesocket(sockfd);


	WSACleanup();//和WSAStartup匹配
	std::cout << "已退出，任务结束" << std::endl;
	getchar();
	return 0;
}






