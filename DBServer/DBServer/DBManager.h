#pragma once
class DBManager
{
public:
	DBManager();

	//���� �˻�
	bool SearchAccount(const char* ID, const char* PW);
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

