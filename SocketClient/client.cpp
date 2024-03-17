#define WIN32_LEAN_AND_MEAM
#include<iostream>
#include<WinSock2.h>//����Socket��ͷ�ļ�
#include<windows.h>//����windows��ͷ�ļ�
#pragma comment(lib,"ws2_32.lib")//���뾲̬���ӿ�


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
		//4 ��������
		//��������
		char cmdBuf[128] = {};
		std::cin >> cmdBuf;
		if (strcmp(cmdBuf, "exit") == 0) {
			std::cout << "�յ��˳�����" << std::endl;
			break;
		}
		else {
			send(sockfd, cmdBuf, strlen(cmdBuf) + 1, 0);
		}

		//5 ������Ӧ
		char recvBuf[128] = {};
		int nLen = recv(sockfd, recvBuf, 128, 0);
		if (nLen > 0) {
			std::cout << "���յ����ݣ�" << recvBuf << std::endl;
		}

	}
	//6 �ر��׽���
	closesocket(sockfd);
	WSACleanup();//��WSAStartupƥ��
	std::cout << "���˳����������" << std::endl;
	getchar();
	return 0;
}