#include"stdafx.h"

void PlayerManager::Update(float dTime)
{
	for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
	{
		i->second->Update(dTime);
	}
}

void PlayerManager::Write(OutputMemoryStream& os)
{
	short playerCount = GetUserCount();
	os.Write(playerCount);
	if (playerCount != 0)
	{
		for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
		{
			i->second->Write(os);
		}
	}
}

void PlayerManager::UpdateRead(InputMemoryStream& is)
{
	std::string _name;
	is.Read(_name);

	//�÷��̾� ����Ʈ�� ������ ����Ʈ�� ������
	auto FindName = PlayerList.find(_name);
	if (PlayerList.end() != FindName)
	{
		PlayerList[_name]->Read(is);
	}
	else
	{
		Player* _player = new Player();
		PlayerData data;
		data.Init(_name, 5, 100, 1);
		_player->SetData(data);
		PlayerList.insert(std::make_pair(_name, _player));
		PlayerList[_name]->Read(is);
	}
}

void PlayerManager::DeleteRead(InputMemoryStream& is)
{
	std::string _name;
	is.Read(_name);

	auto FindName = PlayerList.find(_name);
	if (PlayerList.end() != FindName)
	{
		PlayerList.erase(_name);
		std::cout << _name << " ��������" << std::endl;
	}
}