#include "stdafx.h"


int main()
{
	std::cout << "���� �۵���" << std::endl;
	_IOCP.Initialize();

	Monster monster;

	Vector3 dir;
	dir.Set(0, 0, 1);
	monster.SetVelocity(dir);
	while (true)
	{
		//���ӷ���
		float a = 0;
		a = _FrameManager.GetTime();

		monster.Update();
	}

	return 0;
}