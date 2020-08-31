#pragma once
class DBManager : public GlobalSingleton<DBManager>
{
public:
	DBManager();

	//���� �˻�
	bool SearchAccount(InputMemoryStream& inInputStream,int nSessionID);
private:
	Database db;
	Table tbl;

	//���� ����
	char ErrStr[200];

	//ODBC�� �����ϱ� ���� ���ڿ�
	char CnnStr[200] = "DRIVER={MySQL ODBC 8.0 ANSI Driver};\
				   SERVER=localhost;\
				   DATABASE=iocpserver;\
				   USER=root;\
				   PASSWORD=@ppgk38629;";
};

#define _DBManager DBManager::Singleton()