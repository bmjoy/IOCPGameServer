#include"stdafx.h"

IOCP::IOCP()
{
	InitSocket();
	BindAndListen(PORTNUM);

	for (size_t i = 0; i < MAX_CLIENT_COUNT; ++i)
	{
		ServerSession *session = new ServerSession(i);
		m_SessionList.push_back(session);
		m_SessionQueue.push_back(i);
	}
}


IOCP::~IOCP()
{
	WSACleanup();

	for (size_t i = 0; i < m_SessionList.size(); ++i)
	{
		if (m_SessionList[i]->m_Socket != INVALID_SOCKET)
		{
			m_SessionList[i]->CloseSocket();
		}
		delete m_SessionList[i];
	}

	for (int i = 0; i < MAX_WORKTHREAD; ++i)
	{
		PostQueuedCompletionStatus(m_hIOCP, 0, 0, NULL);
	}
	for (int i = 0; i < MAX_WORKTHREAD; ++i)
	{
		CloseHandle(m_WorkThread[i]);
		WaitForSingleObject(m_WorkThread[i], INFINITE);
	}

	if (m_Socket != INVALID_SOCKET)
	{
		shutdown(m_Socket, SD_BOTH);
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
}

bool IOCP::InitSocket()
{
	WSADATA wsaData;

	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		std::cout << "WinSock2.2 DLL �ε忡 �����Ͽ����ϴ�" << std::endl;
		return false;
	}

	m_Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (m_Socket == INVALID_SOCKET)
	{
		std::cout << "���� ���� ����" << std::endl;
	}

	int option = FALSE;
	setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_hIOCP == NULL)
		return false;

	return true;
}

bool IOCP::BindAndListen(int Port)
{
	SOCKADDR_IN m_SocketAddr;
	ZeroMemory(&m_SocketAddr, sizeof(m_SocketAddr));
	m_SocketAddr.sin_family = AF_INET;
	m_SocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_SocketAddr.sin_port = htons(Port);

	int ret = bind(m_Socket, (SOCKADDR*)&m_SocketAddr,sizeof(SOCKADDR_IN));
	
	if (ret != 0)
	{
		std::cout << "bind ����" << std::endl;
		return false;
	}

	ret = listen(m_Socket, SOMAXCONN);
	if (ret != 0)
	{
		std::cout << "listen ����" << std::endl;
		return false;
	}

	return true;
}

bool IOCP::Initialize()
{
	bool ret = CreateAcceptThread();
	if (ret == false)
		return false;

	ret = CreateWorkThread();
	if (ret == false)
		return false;

	std::cout << "ServerStart!!" << std::endl;
}

unsigned int WINAPI CallAcceptThread(LPVOID p)
{
	IOCP* pOverlappedEvent = (IOCP*)p;
	pOverlappedEvent->AcceptThread();
	return 0;
}

bool IOCP::CreateAcceptThread()
{
	unsigned int ThreadId = 0;
	
	//CREATE_SUSPEND : ResumThread�� ȣ���ϱ� ������ �����带 �������� �ʴ´�
	m_AcceptThread = (HANDLE)_beginthreadex(NULL, 0, &CallAcceptThread, this, CREATE_SUSPENDED, &ThreadId);
	if (m_AcceptThread == NULL)
	{
		std::cout << "Accept ������ ��������" << std::endl;
		return false;
	}
	ResumeThread(m_AcceptThread);
	return true;
}

unsigned int WINAPI CallWorkThread(LPVOID p)
{
	IOCP* pOverlappedEvent = (IOCP*)p;
	pOverlappedEvent->WorkThread();
	return 0;
}

bool IOCP::CreateWorkThread()
{
	unsigned int ThreadID = 0;
	for (size_t i = 0; i < MAX_WORKTHREAD; ++i)
	{
		m_WorkThread[i] = (HANDLE)_beginthreadex(NULL, 0, &CallWorkThread, this, CREATE_SUSPENDED, &ThreadID);
		if (m_WorkThread == NULL)
		{
			std::cout << "Work ������ ��������" << std::endl;
			return false;
		}
		ResumeThread(m_WorkThread[i]);
	}

	return true;
}

void IOCP::AcceptThread()
{
	SOCKADDR_IN ClientAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);

	std::cout << "Accept Start" << std::endl;

	while (true)
	{
		/*	if (m_SessionQueue.size() == 0)
			{
				std::cout << "Client is FULL!!" << std::endl;
				return;
			}*/

			//���� ť���ִ� id�� �����´�
		int SessionID = m_SessionQueue.front();

		m_SessionList[SessionID]->m_Socket = accept(m_Socket, (SOCKADDR*)&ClientAddr, &nAddrLen);
		if (m_SessionList[SessionID]->m_Socket == INVALID_SOCKET)
			break;


		m_SessionList[SessionID]->SetBuffer();

		//������ CreateIoCompletionPort�� �ִ´� ������ �����ʱ� ���Ͽ� CriticalSection�� �Ǵ�
		CSLOCK(m_csSockObj)
		{
			HANDLE hIOCP;

			hIOCP = CreateIoCompletionPort((HANDLE)m_SessionList[SessionID]->m_Socket,
				m_hIOCP,
				(ULONG_PTR)(m_SessionList[SessionID]),
				0);

			if (hIOCP == NULL || m_hIOCP != hIOCP)
			{
				std::cout << "IOCP ���� ����!!" << std::endl;
				return;
			}
		}

		bool ret = m_SessionList[SessionID]->RecvPacket();
		if (ret == false)
		{
			return;
		}
		//���������� accept�� ������ �տ� �ִ� �����͸� ������
		m_SessionQueue.pop_front();

		std::cout << "���ο� Ŭ���̾�Ʈ ����!!! ���� ���̵�� : " << SessionID << std::endl;
		std::cout << "ť�� ���� ������ : " << m_SessionQueue.size() << std::endl;
	}
}

void IOCP::WorkThread()
{
	ServerSession* pServerSession = NULL;
	bool result = true;
	DWORD ioSize = 0;
	Buffer* pBuffer = NULL;

	while (true)
	{
		result = GetQueuedCompletionStatus(
			m_hIOCP,
			&ioSize,
			(PULONG_PTR)&pServerSession,
			(LPOVERLAPPED *)&pBuffer,
			INFINITE);

		if (result == TRUE && ioSize == 0 && pBuffer == NULL)
		{
			break;
		}

		//Ŭ���̾�Ʈ ����
		if (result == false || ioSize == 0)
		{
			CloseSession(pServerSession->GetSessionID());
			continue;
		}
		
		if (pBuffer == NULL)
			continue;

		switch (pBuffer->state)
		{
		case SEND:
			break;
		case RECV:
			pServerSession->Handle_Receive(pBuffer->cBuffer, ioSize);
			//�����͸� ���� ����
			pServerSession->RecvPacket();
		
			break;
		}
		
	}
}

void IOCP::SendAllPlayer(const char* data, int size)
{
	for (int i = 0; i < m_SessionList.size(); ++i)
	{
		if (m_SessionList[i]->m_Socket != INVALID_SOCKET && m_SessionList[i]->GetSendOk())
		{
			m_SessionList[i]->SendPacket(data, size);
		}
	}
}

void IOCP::SendPlayer(int nSessionID,const char* data, int size)
{
	m_SessionList[nSessionID]->SendPacket(data, size);
}

void IOCP::SetSessionSendOk(int nSessionID)
{
	m_SessionList[nSessionID]->SetSendOk(true);
}

void IOCP::CloseSession(int nSessionID)
{
	std::cout << "Ŭ���̾�Ʈ ���� : " << nSessionID << std::endl;
	m_SessionList[nSessionID]->CloseSocket();
	//����� Ŭ���̾�Ʈ ���� ID�� ������
	m_SessionQueue.push_back(nSessionID);
}