#pragma once

template<typename T>
class GlobalSingleton
{
public:
	inline static T& Singleton() { return SingletonCreator::Access(); }
	inline			 GlobalSingleton() {}
private:
	GlobalSingleton(const GlobalSingleton&);
	GlobalSingleton& operator=(const GlobalSingleton&);
protected:
	class SingletonCreator
	{
	public:
		inline SingletonCreator()
		{
			Access();
		}

		inline static T& Access()
		{
			/*
			���� ��ü�� �ζ��� �Լ��� ����� ���
			�� �̻��� ��ü�� ���� �� �ִ�.
			�� ������ �ذ��Ϸ��� ���ο� ���� �ν��Ͻ��ִ� �� �ζ��� ��ɿ� �������� �ؾ��Ѵ�.
			*/
			static T& ref = _CreateUniqueInstance();
			return ref;
		}

	private:
		inline void _ToGuaranteeInstanceCreationBeforeMainFunction() {}

		__declspec(noinline)static T& _CreateUniqueInstance()
		{
			static T obj;
			//�ǹ� ���� ��� �Լ��� ȣ���Ͽ� 'ms_SingletonCreator'�� �����ؾ� �Ѵ�.
			//'T' ������ 'obj'�� main()���� ���� �����ȴٴ� ���̴�.
			ms_SingletonCreator._ToGuaranteeInstanceCreationBeforeMainFunction();
			return obj;
		}
	private:
		static SingletonCreator ms_SingletonCreator;
	};
};

template<typename T>
typename GlobalSingleton<T>::SingletonCreator GlobalSingleton<T>::SingletonCreator::ms_SingletonCreator;