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


int main()
{
	//����Windows socket2.x����
	WORD ver = MAKEWORD(2, 2);//()���ǰ汾��
	WSADATA dat;
	WSAStartup(ver, &dat);//����win_Socket���绷��
	//1 �����׽���
	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) {
		std::cout << "error,����socketʧ��" << std::endl;
	}
	else {
		std::cout << "����socket�ɹ�" << std::endl;
	}
	//2 ׼����ַ�ṹ
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//3 ��������
	int ret = connect(sockfd, (const sockaddr*)&sin, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR) {
		std::cout << "error,���ӷ�����ʧ��" << std::endl;
	}
	else {
		std::cout << "���ӷ������ɹ�" << std::endl;
	}
	
	while (true) {

		//��������
		char cmdBuf[128] = {};
		std::cin >> cmdBuf;
		if (strcmp(cmdBuf, "exit") == 0) {
			std::cout << "�յ��˳�����" << std::endl;
			break;
		}
		else if (strcmp(cmdBuf, "login") == 0) {
			Login login = { "ns","ns123" };
			DataHeader dh = { sizeof(login),CMD_LOGIN };
			//���������������
			send(sockfd, (const char*)&dh, sizeof(DataHeader), 0);
			send(sockfd, (const char*)&login, sizeof(Login), 0);
			//���շ��������ص�����
			DataHeader retHeader = {};
			LoginResult retLogin = {};
			recv(sockfd, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(sockfd, (char*)&retLogin, sizeof(LoginResult), 0);
			std::cout << "LoginResult: " << retLogin._result << std::endl;
		}
		else if (strcmp(cmdBuf, "logout") == 0) {
			Logout logout = { "ns" };
			DataHeader dh = { sizeof(logout),CMD_LOGOUT };
			//���������������
			send(sockfd, (const char*)&dh, sizeof(DataHeader), 0);
			send(sockfd, (const char*)&logout, sizeof(Logout), 0);
			//���շ��������ص�����
			DataHeader retHeader = {};
			LogoutResult retLogout = {};
			recv(sockfd, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(sockfd, (char*)&retLogout, sizeof(LogoutResult), 0);
			std::cout << "LoginResult: " << retLogout._result << std::endl;

		}
		else {
			std::cout << "��֧������������" << std::endl;
		}
	}
	//6 �ر��׽���
	closesocket(sockfd);
	WSACleanup();//��WSAStartupƥ��
	std::cout << "���˳����������" << std::endl;
	getchar();
	return 0;
}