#define WIN32_LEAN_AND_MEAM
#include<iostream>
#include<WinSock2.h>//����Socket��ͷ�ļ�
#include<windows.h>//����windows��ͷ�ļ�
#pragma comment(lib,"ws2_32.lib")//���뾲̬���ӿ�
#include<vector>
//���͵�������Ϣ���ݽṹ��
//struct DataPackage {
//	int age;
//	char name[32];
//};

//���籨�����ݸ�ʽ�ṹ
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
//��ͷ ������Ϣ���Ĵ�С���������ݵ�����
struct DataHeader {
	short _dataLength;
	short _cmd;
};
//���� �������
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

//����ͷ��˷��͵�������
int processor(SOCKET cSock) {
	//�������ݻ�����
	char szRecv[1024] = {};
	int nLen = recv(cSock, (char*)&szRecv, sizeof(DataHeader), 0);
	//�ٷŵ�����ͷ
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		std::cout << "�ͻ���Socket = "<<cSock<<" ���˳����������" << std::endl;
		return -1;
	}

	// 6 ��ͻ��˷�����Ӧ 
	switch (header->_cmd)
	{
	case CMD_LOGIN: {
		Login login = {};
		recv(cSock, (char*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
		std::cout << "�յ��ͻ���Socket = "<<cSock<<" ���CMD_LOGIN" << " ���ݳ��ȣ�" << login._dataLength
			<< " �û�����" << login._userName << " ���룺" << login._password << std::endl;

		LoginResult ret;
		send(cSock, (char*)&ret, sizeof(LoginResult), 0);

	}break;
	case CMD_LOGOUT: {
		Logout logout = {};
		recv(cSock, (char*)&logout + sizeof(DataHeader), sizeof(Logout) - sizeof(DataHeader), 0);
		std::cout << "�յ��ͻ���Socket = " << cSock << " ���CMD_LOGOUT" << " ���ݳ��ȣ�" << logout._dataLength
			<< " �û�����" << logout._userName << std::endl;
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
	
   

	// �ȴ����Ӻͽ����������ѭ��
	while (true) {

		//select����ģ������������
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
		//����socket����
		int ret = select(sockfd+1, &fdRead, &fdWrite, &fdExp, &t);
		//nfds ��һ������ֵ����ָfd_set����������������(socket)������������
		//���������ļ������������ֵ ��windows�������������Ϊ0
		if (ret < 0) {
			std::cout << "select�������" << std::endl;
			break;
		}
		//��ѯsockfd�Ƿ���fdRead������
		if (FD_ISSET(sockfd, &fdRead)) {
			FD_CLR(sockfd, &fdRead);
			// 4 �ȴ����ܿͻ������� accept 
			sockaddr_in clientAddr = {};
			int cAddrLen = sizeof(sockaddr_in);
			SOCKET cSock = INVALID_SOCKET;
			cSock = accept(sockfd, (sockaddr*)&clientAddr, &cAddrLen);
			if (cSock == INVALID_SOCKET) {
				std::cout << "error,���յ���Ч�ͻ���SOCKET" << std::endl;
			}
			//�������ͻ��˷����¿ͻ��˼������Ϣ
			for (int n = (int)g_clients.size() - 1; n >= 0; n--) {
				NewUserJoin userJoin;
				send(g_clients[n], (const char*)&userJoin, sizeof(NewUserJoin), 0);
			}
			//�ͻ��˼���g_clients
			g_clients.push_back(cSock);
			std::cout << "�¿ͻ��˼��룺" << "socket=" << (int)cSock << " IP= " << inet_ntoa(clientAddr.sin_addr) << std::endl;
		}
		//���ô���������
		for (int n = 0; n < (int)fdRead.fd_count; n++) {
			if (processor(fdRead.fd_array[n]) == -1) {
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end()) {
					g_clients.erase(iter);
				}
			}
		}

		//����ҵ��

		
	}
  
	for (int n = (int)g_clients.size() - 1; n >= 0; n--) {
		closesocket(g_clients[n]);
	}

    // 7 �ر�socket  closesocket
	closesocket(sockfd);


	WSACleanup();//��WSAStartupƥ��
	std::cout << "���˳����������" << std::endl;
	getchar();
	return 0;
}






