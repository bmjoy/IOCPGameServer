#pragma once
class GameObjectManager:public GlobalSingleton<GameObjectManager>
{
public:
	void Init();
	void Update();
	void DeletePlayer();
	void SendToClientPacket();

	void UpdatePlayerManager(InputMemoryStream& is);
	void DeletePlayerManager(InputMemoryStream& is);
	void SendToClientPacket(int nSessionID);
	void Write(OutputMemoryStream os);
	void Read(InputMemoryStream& is);

private:
	PlayerManager *playerManager;
	MonsterManager *monsterManager;

	float dTime = 0;
	float SendTime = 0;
};

#define _GameObjectManager GameObjectManager::Singleton()