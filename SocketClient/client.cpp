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
	NewUserJoin() {
		_dataLength = sizeof(NewUserJoin);
		_cmd = CMD_NEW_USER_JOIN;
		sock = 1;
	}
	int sock;
};

//������������͵�������
int processor(SOCKET cSock) {
	//�������ݻ�����
	char szRecv[1024] = {};
	int nLen = recv(cSock, (char*)&szRecv, sizeof(DataHeader), 0);
	//�ٷŵ�����ͷ
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		std::cout << "�����˶Ͽ�����" << std::endl;
		return -1;
	}

	switch (header->_cmd)
	{
	case CMD_LOGIN_RESULT: {
		recv(cSock, szRecv + sizeof(DataHeader), header->_dataLength - sizeof(DataHeader), 0);
		LoginResult *ret=(LoginResult*)szRecv;
		std::cout << "�յ��������Ϣ��CMD_LOGIN_RESULT�����ݳ��ȣ�" << ret->_dataLength << std::endl;

	}break;
	case CMD_LOGOUT_RESULT: {
		recv(cSock, szRecv + sizeof(DataHeader), header->_dataLength - sizeof(DataHeader), 0);
		LogoutResult* ret = (LogoutResult*)szRecv;
		std::cout << "�յ��������Ϣ��CMD_LOGOUT_RESULT�����ݳ��ȣ�" << ret->_dataLength << std::endl;

	}break;
	case CMD_NEW_USER_JOIN: {
		recv(cSock, szRecv + sizeof(DataHeader), header->_dataLength - sizeof(DataHeader), 0);
		NewUserJoin* ret = (NewUserJoin*)szRecv;
		std::cout << "�յ��������Ϣ��CMD_NEW_USER_JOIN�����ݳ��ȣ�" << ret->_dataLength << std::endl;
	}break;
	}
}



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

		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(sockfd, &fdReads);
		timeval t = { 1,0 };
		int ret = select(sockfd, &fdReads, 0, 0, &t);
		if (ret < 0) {
			std::cout << "select�������1" << std::endl;
			break;
		}
		if (FD_ISSET(sockfd, &fdReads)) {
			FD_CLR(sockfd, &fdReads);
			//���ô�����
			if (processor(sockfd) == -1) {
				std::cout << "select�������2" << std::endl;
				break;
			}

		}

		//����ʱ�䴦������ҵ��

		Login login;
		strcpy(login._userName, "Nishuang");
		strcpy(login._password, "nishuang123");
		send(sockfd, (const char*)&login, sizeof(login), 0);



	}
	//6 �ر��׽���
	closesocket(sockfd);
	WSACleanup();//��WSAStartupƥ��
	std::cout << "���˳����������" << std::endl;
	getchar();
	return 0;
}