#define WIN32_LEAN_AND_MEAM
#include<iostream>
#include<WinSock2.h>//����Socket��ͷ�ļ�
#include<windows.h>//����windows��ͷ�ļ�
#pragma comment(lib,"ws2_32.lib")//���뾲̬���ӿ�
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
	char recvBuf[128] = {};//�������ݻ�����
	while (true) {
		int nLen = recv(cSock, recvBuf, 128, 0);
		if (nLen <= 0) {
			std::cout << "�ͻ������˳����������" << std::endl;
			break;
		}
		std::cout << "�յ��ͻ������" << recvBuf << std::endl;
		// 6 ��ͻ��˷�����Ӧ 
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
  
    // 7 �ر�socket  closesocket
	closesocket(sockfd);


	WSACleanup();//��WSAStartupƥ��
	std::cout << "���˳����������" << std::endl;
	getchar();
	return 0;
}






