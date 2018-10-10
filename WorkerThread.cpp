#include "WorkerThread.h"
#include "ThreadLocal.h"
#include <iostream>

WorkerThread::WorkerThread(int id, const std::wstring& name )
	: Thread(id, name)
	
{
	std::atomic_init(&isContinue_, true);
}

WorkerThread::~WorkerThread()
{
	isContinue_.exchange(false);
	Wait(WAIT_FINISHED_TIME);
}

bool WorkerThread::Init()
{
	if (!Thread::Init())
	{
		return false;
	}

	Initialize_TLS(GetId());

	return true;
}

void WorkerThread::Run()
{

	isRunning_.exchange(true);

	while (isContinue_ == true)
	{
		//TODO // 스레드가 할일  
		
		std::cout << "My ThreadID : " << LThreadId << std::endl;

		
		
	}

	isRunning_.exchange(false);
}

void WorkerThread::Stop()
{
	isContinue_.exchange(false);
}

void WorkerThread::Shutdown()
{
	Terminate_TLS();
}