#pragma once
#include<cstdint>
#include<cstdlib>

class OutputMemoryStream
{
public:
	OutputMemoryStream() :
		mHead(2),	//����� �ֱ����� short�� ũ��� 2��Ʈ �̱� ����
		mBuffer(nullptr)
	{
		ReallocBuffer(1500 * 8);
	}

	~OutputMemoryStream() { std::free(mBuffer); }

	const char* GetBufferPtr()	const { return mBuffer; }
	int GetDataLength()			const { return mHead; }

	template<typename T>
	void Write(T inData, int inDataCount = sizeof(T))
	{
		static_assert(std::is_arithmetic<T>::value ||
			std::is_enum<T>::value,
			"�Ϲ� �б�� �⺻ ������ ���� �� �����մϴ�");

		(*(T*)(mBuffer + mHead)) = inData;
		mHead += inDataCount;
	}

	void Write(const std::string& inString)
	{
		//string ���� ��쿡�� ����� ��� �� ������ ����ȭ�� �Ҷ� �ش� �����
		//���� ����ϰ� ���ڿ��� ����ȭ�Ѵ�.
		short elementCount = static_cast<short>(inString.size());
		Write(elementCount);
		for (const auto& element : inString)
		{
			Write(element);
		}
	}

	void Write(Vector3 data);

	void SetSize();
private:
	void ReallocBuffer(int inNewDataLength);

	char* mBuffer;
	int mHead;
	int mCapacity;
};

class InputMemoryStream
{
public:
	InputMemoryStream(char* inBuffer, int inDataCount) :
		mBuffer(inBuffer),
		mCapacity(inDataCount),
		mHead(sizeof(short)),
		mIsOwner(false){}

	~InputMemoryStream() { if (mIsOwner) std::free(mBuffer); }

	const char* GetBufferPtr() const { return mBuffer; }
	int GetRemainingDataCount() const { return mCapacity - mHead; }

	void ResetToCapacity(int inDataCapacity) { mCapacity = inDataCapacity; mHead = 0; }

	template<typename T>
	void Read(T& inData, int inDataCount = sizeof(T))
	{
		static_assert(std::is_arithmetic<T>::value ||
			std::is_enum<T>::value,
			"�Ϲ� �б�� �⺻ ������ ���� �� �����մϴ�");

		inData = (*(T*)(mBuffer + mHead));
		mHead += inDataCount;
	}

	void Read(std::string& inString)
	{
		short elementCount;
		Read(elementCount);
		inString.resize(elementCount);
		for (auto& element : inString)
		{
			Read(element);
		}
	}
private:
	char* mBuffer;
	int mCapacity;
	int mHead;
	bool mIsOwner;
};