#pragma once
#include <iostream>
#include <tuple>
#include <functional>
#include "Job.h"
#include "JobQueue.h"
#include "ThreadManager.h"
#include <memory>
#include <vector>
#include <random>
#include "ThreadLocal.h"

#define  TEST_OBJ_COUNT  1
#define  THREAD_COUNT    2

class TestObject : public Jobable
{
private:

	TestObject() {}
	TestObject(int  a)
		: testCount_(a)
	{}

	TestObject(int a, int b)
		:testCount_(a+b)
	{

	}

public:
	
	template <typename... Args>
	static std::shared_ptr<TestObject> Create(Args&&... args)
	{
		return std::shared_ptr<TestObject>(new TestObject(std::forward<Args>(args)...));
	}

	void TestFunc(int a, int b)
	{
		++testCount_;
	}

	void TestFunc2()
	{
		++testCount_;
		DoasyncJob(&TestObject::TestFunc, 1, 2);
	}

	/*void TestFunc3()
	{
		++testCount_;

	}*/

	int Get()
	{
		return testCount_;
	}

private:

	int testCount_ = 0;
};

std::vector<std::shared_ptr<TestObject>> testObjects;

class TestThread : public WorkerThread
{

public:

	TestThread(int id, const std::wstring& name)
		: WorkerThread(id, name)
	{}

	virtual ~TestThread()
	{
		std::cout << "Bye" << std::endl;
	}

protected:

	bool Init() final
	{
		if (!WorkerThread::Init())
		{
			return false;
		}

		return true;
	}

	void Run() final
	{
		while (isContinue_ == true)
		{
			//TODO // 스레드가 할일  

			//testObjects[rand() % TEST_OBJ_COUNT]->DoasyncJob(&TestObject::TestFunc, rand() % 100, rand() % 150);
			//testObjects[rand() % TEST_OBJ_COUNT]->DoasyncJob(&TestObject::TestFunc, rand() % 1, rand() % 1);
			testObjects[rand() % TEST_OBJ_COUNT]->DoasyncJob(&TestObject::TestFunc2);
			//testObjects[rand() % TEST_OBJ_COUNT]->DoasyncJob(&TestObject::TestFunc3);

			if (testObjects[rand() % TEST_OBJ_COUNT]->Get() > 2)
			{
			//	std::cout << "This hread done :  " << LThreadId << std::endl;

				return;
			}

			//Sleep(1); 풀고 하면 잘되는듯..한데..흠..
		}
	}


	void Shutdown() final
	{

	}
};


int main()
{
	

	
	/*to.DoasyncJob(&TestObject::TestFunc, 1, 2);*/
	
	/*push(&to, &TestObject::TestFunc, 1, 2);
	push(&to2, &TestObject::TestFunc, 3, 3);
	
	std::shared_ptr<Job> job;
	while(Pop(job))
	{
		job->Execute();
	}*/

	
	for (int i = 0; i < TEST_OBJ_COUNT; ++i)
	{
		testObjects.push_back(TestObject::Create());
	}
	
	ThreadManager threadManager;
	threadManager.ThreadStart<TestThread>(THREAD_COUNT, L"TestThread");

	
	HANDLE handles[THREAD_COUNT] = { 0, };
	threadManager.GetHandles(handles);


	WaitForMultipleObjects(THREAD_COUNT, handles, true, INFINITE);

	for (auto& i : testObjects)
		std::cout << "TestCount : " << i->Get() << std::endl;

	getchar();
	
	
	return  0;
}