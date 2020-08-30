#pragma once

#define MAX_BUF_SIZE 1024
#define PORTNUM 8082
#define MAX_CLIENT_COUNT 100
#define MAX_WORKTHREAD 2

class ServerSession;

enum BUFFER_STATE
{
	SEND,
	RECV
};

struct Buffer : OVERLAPPED
{
	char cBuffer[MAX_BUF_SIZE];
	WSABUF wsaBuf;
	BUFFER_STATE state;
};

class IOCP : public GlobalSingleton<IOCP>
{
public:
	IOCP();
	~IOCP();

	bool InitSocket();
	bool BindAndListen(int Port);

	void AcceptThread();
	void WorkThread();

	bool Initialize();

	void SendPlayer(int nSessionID, const char* data, int size);
private:
	void CloseSession(const int nSessionID);

	//Accept ������ ����
	bool CreateAcceptThread();
	//Work ������ ����
	bool CreateWorkThread();

private:
	SOCKET		m_Socket;

	HANDLE m_hIOCP;
	HANDLE m_AcceptThread = NULL;
	HANDLE m_WorkThread[MAX_WORKTHREAD] = { NULL, };

	std::vector<ServerSession*> m_SessionList;	//���� �迭
	std::deque<int> m_SessionQueue;

	CriticalSection m_csSockObj;
};

#define _IOCP IOCP::Singleton()
