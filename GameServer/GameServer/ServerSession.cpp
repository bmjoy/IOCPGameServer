#include "stdafx.h"



ServerSession::ServerSession(int m_index)
{
	this->m_index = m_index;
	ZeroMemory(&m_RecvBuffer, sizeof(Buffer));
	ZeroMemory(&m_SendBuffer, sizeof(Buffer));
	m_RecvBuffer.state = RECV;
	m_SendBuffer.state = SEND;
	m_Socket = INVALID_SOCKET;
	//InitSocket();
}


ServerSession::~ServerSession()
{
}

bool ServerSession::InitSocket()
{
	WSADATA wsaData;

	//! [WinSock 2.2] LOAD 
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nRet != 0)
	{
		std::cout << "Failed WinSock2.2 DLL Load!\n";
		return false;
	}

	//! Create Socket 
	m_Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_Socket == INVALID_SOCKET)
	{
		std::cout << "���� ���� ����!\n";
		return false;
	}

	return true;
}

void ServerSession::SetBuffer()
{
	memset(&m_RecvBuffer, 0, sizeof(Buffer));
	memset(&m_SendBuffer, 0, sizeof(Buffer));
}

bool ServerSession::SendPacket(const char* data,int size)
{
	CriticalSectionLock lock(m_csSock);

	m_SendBuffer.wsaBuf.len = size;
	m_SendBuffer.wsaBuf.buf = (char*)data;
	DWORD dwRecvNumBytes = 0;

	//SendPacket
	WSASend(m_Socket,
		&(m_SendBuffer.wsaBuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED)&m_SendBuffer,
		NULL
	);

	return true;
}

bool ServerSession::RecvPacket()
{
	CriticalSection lock(m_csSock);
	DWORD dwFlag = 0;
	DWORD dwRecvNumByte = 0;

	m_RecvBuffer.wsaBuf.len = MAX_BUF_SIZE;
	m_RecvBuffer.wsaBuf.buf = m_RecvBuffer.cBuffer;
	m_RecvBuffer.state = RECV;

	int result = WSARecv(m_Socket,
		&(m_RecvBuffer.wsaBuf),
		1,
		&dwRecvNumByte,
		&dwFlag,
		(LPWSAOVERLAPPED)&(m_RecvBuffer),
		NULL);

	if (result == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		std::cout << "Recv ����" << std::endl;
		return true;
	}
	return true;
}

void ServerSession::Handle_Receive(const char* data, int size)
{
	/*
	��Ʈ��ũ Ư���� Ŭ���̾�Ʈ���� ������ �����͸������� ��û�� ���ÿ� ���� �� �ϸ�, ���������� Ŭ���̾�Ʈ���� ������ ������� ���� �ʴ´�.
	�Ѳ����� ��� ���� ���� �ְ� ���� �� ������ ���� �� �ִ�.(��, Ŭ���̾�Ʈ�� Send�� �� �� �ߴµ�, ���������� Receive�� �� ���� �߻��Ѵ�.)
	�̷�, ��츦 ó���ϱ� ���ؼ� ���� ���� ������ mPacketBuffeer�� ������ �� Ŭ���̾�Ʈ�� ���ÿ� ������ ��û�ϸ鼭 ������ �� ���� �� �����Ƿ�,
	�� ��û���� ������ ó���Ѵ�. �׸��� Ŭ���̾�Ʈ�� ���� �������� �Ϻ� ������ ���, �켱�� ó������ �ʰ� ���ܳ��Ҵٰ� ������ ���� �����Ϳ� �����Ͽ�
	ó���Ѵ�.
	*/

	//���� �����͸� ��Ŷ ���ۿ� ����
	memcpy(&mPacketBuffer[mPacketBufferMark], data, size);

	int nPacketData = mPacketBufferMark + size;
	int nReadData = 0;

	while (nPacketData > 0)	//���� �����͸� ��� ó���� ������ �ݺ�
	{
		//���� �����Ͱ� ��Ŷ �ش����� ������ �ߴ�
		if (nPacketData < sizeof(PacketHeader))
		{
			break;
		}

		PacketHeader* pHeader = (PacketHeader *)&mPacketBuffer[nReadData];

		if (pHeader->size <= nPacketData)	//ó���� �� �ִ� ��ŭ �����Ͱ� �ִٸ� ��Ŷ�� ó��
		{
			InputMemoryStream io((char*)data, pHeader->size);
			_PacketProc.CheckPacket(io, GetSessionID());
			nPacketData -= pHeader->size;
			nReadData += pHeader->size;
		}
		else
		{
			break;	//��Ŷ���� ó���� �� ���� ��ŭ�� �ƴϸ� �ߴ�
		}
	}

	if (nPacketData > 0)
	{
		char TempBuffer[MAX_RECEIVE_BUFFER_LEN * 2] = { 0, };
		memcpy(&TempBuffer[0], &mPacketBuffer[nReadData], nPacketData);
		memcpy(&mPacketBuffer[0], &TempBuffer[0], nPacketData);
	}

	//���� ������ ���� �����ϰ� ������ �ޱ� ��û
	mPacketBufferMark = nPacketData;
}

void ServerSession::CloseSocket()
{
	CSLOCK(m_csSock)
	{
		if (m_Socket != INVALID_SOCKET)
		{
			shutdown(m_Socket, SD_BOTH);
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}
	}
}