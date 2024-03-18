#define WIN32_LEAN_AND_MEAM
#include<iostream>
#include<WinSock2.h>//包含Socket的头文件
#include<windows.h>//包含windows的头文件
#pragma comment(lib,"ws2_32.lib")//加入静态链接库

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
	NewUserJoin() {
		_dataLength = sizeof(NewUserJoin);
		_cmd = CMD_NEW_USER_JOIN;
		sock = 1;
	}
	int sock;
};

//处理服务器发送的请求函数
int processor(SOCKET cSock) {
	//接收数据缓冲区
	char szRecv[1024] = {};
	int nLen = recv(cSock, (char*)&szRecv, sizeof(DataHeader), 0);
	//再放到数据头
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		std::cout << "与服务端断开连接" << std::endl;
		return -1;
	}

	switch (header->_cmd)
	{
	case CMD_LOGIN_RESULT: {
		recv(cSock, szRecv + sizeof(DataHeader), header->_dataLength - sizeof(DataHeader), 0);
		LoginResult *ret=(LoginResult*)szRecv;
		std::cout << "收到服务端消息：CMD_LOGIN_RESULT，数据长度：" << ret->_dataLength << std::endl;

	}break;
	case CMD_LOGOUT_RESULT: {
		recv(cSock, szRecv + sizeof(DataHeader), header->_dataLength - sizeof(DataHeader), 0);
		LogoutResult* ret = (LogoutResult*)szRecv;
		std::cout << "收到服务端消息：CMD_LOGOUT_RESULT，数据长度：" << ret->_dataLength << std::endl;

	}break;
	case CMD_NEW_USER_JOIN: {
		recv(cSock, szRecv + sizeof(DataHeader), header->_dataLength - sizeof(DataHeader), 0);
		NewUserJoin* ret = (NewUserJoin*)szRecv;
		std::cout << "收到服务端消息：CMD_NEW_USER_JOIN，数据长度：" << ret->_dataLength << std::endl;
	}break;
	}
}



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

		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(sockfd, &fdReads);
		timeval t = { 1,0 };
		int ret = select(sockfd, &fdReads, 0, 0, &t);
		if (ret < 0) {
			std::cout << "select任务结束1" << std::endl;
			break;
		}
		if (FD_ISSET(sockfd, &fdReads)) {
			FD_CLR(sockfd, &fdReads);
			//调用处理函数
			if (processor(sockfd) == -1) {
				std::cout << "select任务结束2" << std::endl;
				break;
			}

		}

		//空闲时间处理其他业务

		Login login;
		strcpy(login._userName, "Nishuang");
		strcpy(login._password, "nishuang123");
		send(sockfd, (const char*)&login, sizeof(login), 0);



	}
	//6 关闭套接字
	closesocket(sockfd);
	WSACleanup();//和WSAStartup匹配
	std::cout << "已退出，任务结束" << std::endl;
	getchar();
	return 0;
}