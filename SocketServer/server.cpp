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
	while (true) {
		DataHeader header = {};//接收数据放到数据头
		int nLen = recv(cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0) {
			std::cout << "客户端已退出，任务结束" << std::endl;
			break;
		}

		// 6 向客户端发送响应 
		switch (header._cmd)
		{
		case CMD_LOGIN: {
			Login login = {};
			recv(cSock, (char*)&login+sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);
			std::cout << "收到命令：CMD_LOGIN" << " 数据长度：" << login._dataLength 
				<< " 用户名：" << login._userName << " 密码：" << login._password << std::endl;

			LoginResult ret;
			send(cSock, (char*)&ret, sizeof(LoginResult), 0);

		}break;
		case CMD_LOGOUT: {
			Logout logout = {};
			recv(cSock, (char*)&logout+sizeof(DataHeader), sizeof(Logout)-sizeof(DataHeader), 0);
			std::cout << "收到命令：CMD_LOGOUT" << " 数据长度：" << logout._dataLength
				<< " 用户名：" << logout._userName << std::endl;
			LogoutResult ret;
			send(cSock, (char*)&ret, sizeof(LogoutResult), 0);
		}break;
		default:
			header._cmd = CMD_ERROR;
			header._dataLength = 0;
			send(cSock, (char*)&header, sizeof(DataHeader), 0);
			break;
		}
	}
  
    // 7 关闭socket  closesocket
	closesocket(sockfd);


	WSACleanup();//和WSAStartup匹配
	std::cout << "已退出，任务结束" << std::endl;
	getchar();
	return 0;
}






