#define WIN32_LEAN_AND_MEAM
#include<iostream>
#include<WinSock2.h>//����Socket��ͷ�ļ�
#include<windows.h>//����windows��ͷ�ļ�
#pragma comment(lib,"ws2_32.lib")//���뾲̬���ӿ�

//���͵�������Ϣ���ݽṹ��
//struct DataPackage {
//	int age;
//	char name[32];
//};

//���籨�����ݸ�ʽ�ṹ
enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};
//��ͷ ������Ϣ���Ĵ�С���������ݵ�����
struct DataHeader {
	short _dataLength;
	short _cmd;
};
//���� �������
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


//��Socket API��������TCP�����
int main() {
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	// 1 ����һ��socket
	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) {
		std::cout << "error,����socketʧ��" << std::endl;
	}
	else {
		std::cout << "����socket�ɹ�" << std::endl;
	}
    // 2 �󶨽��ܿͻ������ӵĶ˿� bind 
	sockaddr_in sin = {}; //�����ַ�ṹ
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (bind(sockfd, (const sockaddr*)&sin, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		std::cout << "error,���������ӿͻ��˵�����˿�ʧ��" << std::endl;
	}
	else
	{
		std::cout << "�󶨶˿ڳɹ�" << std::endl;
	}
    // 3 ��������˿� listen 
	if (listen(sockfd, 1024) == SOCKET_ERROR) {
		std::cout << "error,��������˿�ʧ��" << std::endl;
	}
	else {
		std::cout << "��������˿ڳɹ�" << std::endl;
	}
	
    // 4 �ȴ����ܿͻ������� accept 
	sockaddr_in clientAddr = {};
	int cAddrLen = sizeof(sockaddr_in);
	SOCKET cSock = INVALID_SOCKET;
	cSock = accept(sockfd, (sockaddr*)&clientAddr, &cAddrLen);
	if (cSock == INVALID_SOCKET) {
		std::cout << "error,���յ���Ч�ͻ���SOCKET" << std::endl;
	}

	// 5 ��������
	while (true) {
		DataHeader header = {};//�������ݷŵ�����ͷ
		int nLen = recv(cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0) {
			std::cout << "�ͻ������˳����������" << std::endl;
			break;
		}
		std::cout << "�յ��ͻ������" << header._cmd <<" ���ݳ��ȣ�"<<header._dataLength << std::endl;
		// 6 ��ͻ��˷�����Ӧ 
		switch (header._cmd)
		{
		case CMD_LOGIN: {
			Login login = {};
			recv(cSock, (char*)&login, sizeof(login), 0);
			LoginResult ret = { 1 };
			send(cSock, (char*)&header, sizeof(DataHeader), 0);
			send(cSock, (char*)&ret, sizeof(LoginResult), 0);
		}break;
		case CMD_LOGOUT: {
			Logout logout = {};
			recv(cSock, (char*)&logout, sizeof(Logout), 0);
			LogoutResult ret = { 1 };
			send(cSock, (char*)&header, sizeof(DataHeader), 0);
			send(cSock, (char*)&ret, sizeof(LogoutResult), 0);
		}break;
		default:
			header._cmd = CMD_ERROR;
			header._dataLength = 0;
			send(cSock, (char*)&header, sizeof(DataHeader), 0);
			break;
		}
	}
  
    // 7 �ر�socket  closesocket
	closesocket(sockfd);


	WSACleanup();//��WSAStartupƥ��
	std::cout << "���˳����������" << std::endl;
	getchar();
	return 0;
}






