#include "stdafx.h"

void PacketProc::CheckPacket(InputMemoryStream& inInputStream, int nSessionID)
{
	short type;
	inInputStream.Read(type);

	switch(type)
	{
	case USER_DATA:
		std::string Name;
		inInputStream.Read(Name);
		int Level;
		inInputStream.Read(Level);

		std::cout << "�̸� : " << Name << " ���� : " << Level << std::endl;
		break;
	}
}