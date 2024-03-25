#include "TcpClient.hpp"
#include<thread>
bool g_bRun = true;
void cmdThread()
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("退出cmdThread线程\n");
			break;
		}
		else {
			printf("不支持的命令。\n");
		}
	}
}
int main()
{
	const int cCount = 1000;
	EasyTcpClient* client[cCount];
	for (int i = 0; i < cCount; ++i)
	{
		if (!g_bRun)
		{
			return 0;
		}
		client[i] = new EasyTcpClient;
	}
	for(int i = 0; i < cCount; ++i)
	{
		if (!g_bRun)
		{
			return 0;
		}
		client[i]->Connect("127.0.0.1", 4567);
		printf("Connect=%d\n", i);

	}
	//启动UI线程
	std::thread t1(cmdThread);
	t1.detach();
	Login login;
	strcpy(login.userName, "Nishuang");
	strcpy(login.Password, "Nishuang123");
	while (g_bRun)
	{

		for (int i = 0; i < cCount; ++i)
		{
			client[i]->SendData(&login);
		}
		//printf("空闲时间处理其它业务..\n");
		//Sleep(1000);
	}
	for (int i = 0; i < cCount; ++i)
	{
		client[i]->Close();
		delete client[i];
	}
	printf("已退出。\n");
	return 0;
}