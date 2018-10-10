#pragma once

#include <string>
#include <Windows.h>
#include <atomic>

class Thread
{

private:

	int threadId_ = 0;
	unsigned int osId_ = 0;
	std::wstring name_ = L"";
	HANDLE hThread_;
	

protected:

	enum TimeOut
	{
		WAIT_FINISHED_TIME = 10000
	};

	std::atomic_bool isRunning_;


	virtual bool Init();
	virtual void Run() = 0;
	virtual void Shutdown();

public:

	Thread(int id, const std::wstring& name);
	virtual ~Thread();

	void Start();
	void Terminate(unsigned int code);
	bool Wait(DWORD timeout);

		
	int	GetId() const;
	const HANDLE GetHandle() const;

private:

	static unsigned int __stdcall ThreadProc(void* arg);
};


