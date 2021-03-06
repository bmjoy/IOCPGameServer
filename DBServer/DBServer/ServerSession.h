#pragma once
#define MAX_RECEIVE_BUFFER_LEN 1024

struct PacketHeader
{
	short size;
};

class ServerSession
{
public:
	ServerSession(int m_index);
	~ServerSession();

	bool InitSocket();

	bool SendPacket(const char* data, int size);

	bool RecvPacket();
	void Handle_Receive(const char* data, int size);

	void CloseSocket();

	void SetBuffer();

	int GetSessionID() { return m_index; }

	bool GetSendOk() { return m_sendOk; }
	void SetSendOk(bool result) { m_sendOk = result; }
public:
	SOCKET m_Socket;
	Buffer m_RecvBuffer;
	Buffer m_SendBuffer;
private:
	int m_index;
	bool m_sendOk = false;
	int mPacketBufferMark = 0;
	char mPacketBuffer[MAX_RECEIVE_BUFFER_LEN * 2];
	CriticalSection		m_csSock;
};

