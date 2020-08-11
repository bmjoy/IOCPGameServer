#include"stdafx.h"

struct CriticalSection :public CRITICAL_SECTION
{
	CriticalSection() { InitializeCriticalSection(this); }
	~CriticalSection() { DeleteCriticalSection(this); }
};

/*
RAII
 C++ ���� �����ڰ� ���� ���ҽ� ������ ������ϴ� ���� �޸� ������ �����ϱ� ���� �߿��� ������� �ش� ���ҽ��� ��������
 ���� ��쿡 �ڵ����� ������ ���־� ���ܰ� �߻��ϰų� �ϴ� ��쿡�� ȹ���� �ڿ��� �������� �����Ͽ� ������ �ڵ带 �ۼ��� �� �ִ�.
*/

class CriticalSectionLock
{
public:
	CriticalSectionLock(const CRITICAL_SECTION& cs) :m_pcs(&cs)
	{
		assert(m_pcs != nullptr);
		EnterCriticalSection((LPCRITICAL_SECTION)m_pcs);
	}

	~CriticalSectionLock()
	{
		if(m_pcs!=nullptr)
			LeaveCriticalSection((LPCRITICAL_SECTION)m_pcs);
	}

	//explicit : �Ͻ����� ����ȯ�� �������Ͽ� ���
	explicit operator bool() { return true; }
private:
	const CRITICAL_SECTION* m_pcs;
};

//c#������ lock�̶�� ��ũ�θ� �����ؼ� CriticalSection�� ���� ���� ������ ����������
//c++�� ���� ������ ���� ���������Ѵ�
#define CSLOCK(cs_) if(CriticalSectionLock CONCATENATE(lock,__LINE__) = cs_) //__LINE__ : ����Ű� ���Ե� �����Ϸ����� ���� �����ϸ鼭 � ���ο��� ������������ �������
