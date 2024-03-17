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
	CMD_LOGOUT,
	CMD_ERROR
};
//包头 描述消息包的大小，描述数据的作用
struct DataHeader {
	short _dataLength;
	short _cmd;
};
//包体 存放数据
struct Login {
	char _userName[32];
	char _password[32];
};
struct LoginResult {
	int _result;
};
struct Logout
{
	char _userName[32];
};
struct LogoutResult
{
	int _result;
};


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

		//输入请求
		char cmdBuf[128] = {};
		std::cin >> cmdBuf;
		if (strcmp(cmdBuf, "exit") == 0) {
			std::cout << "收到退出命令" << std::endl;
			break;
		}
		else if (strcmp(cmdBuf, "login") == 0) {
			Login login = { "ns","ns123" };
			DataHeader dh = { sizeof(login),CMD_LOGIN };
			//向服务器发送请求
			send(sockfd, (const char*)&dh, sizeof(DataHeader), 0);
			send(sockfd, (const char*)&login, sizeof(Login), 0);
			//接收服务器返回的数据
			DataHeader retHeader = {};
			LoginResult retLogin = {};
			recv(sockfd, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(sockfd, (char*)&retLogin, sizeof(LoginResult), 0);
			std::cout << "LoginResult: " << retLogin._result << std::endl;
		}
		else if (strcmp(cmdBuf, "logout") == 0) {
			Logout logout = { "ns" };
			DataHeader dh = { sizeof(logout),CMD_LOGOUT };
			//向服务器发送请求
			send(sockfd, (const char*)&dh, sizeof(DataHeader), 0);
			send(sockfd, (const char*)&logout, sizeof(Logout), 0);
			//接收服务器返回的数据
			DataHeader retHeader = {};
			LogoutResult retLogout = {};
			recv(sockfd, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(sockfd, (char*)&retLogout, sizeof(LogoutResult), 0);
			std::cout << "LoginResult: " << retLogout._result << std::endl;

		}
		else {
			std::cout << "不支持命令，请求结束" << std::endl;
		}
	}
	//6 关闭套接字
	closesocket(sockfd);
	WSACleanup();//和WSAStartup匹配
	std::cout << "已退出，任务结束" << std::endl;
	getchar();
	return 0;
}