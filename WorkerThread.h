#pragma once
#include "Thread.h"

class WorkerThread : public Thread
{

protected:

	std::atomic_bool isContinue_;
public:

	WorkerThread(int id, const std::wstring& name);
	~WorkerThread();
	void Stop();

protected:

	bool Init() override;
	void Run() override;
	void Shutdown() override;

}; 
