#include "stdafx.h"


int main()
{
	std::cout << "���� �۵���" << std::endl;
	_IOCP.Initialize();

	_GameObjectManager.Init();

	while (true)
	{
		_GameObjectManager.Update();
	}

	return 0;
}