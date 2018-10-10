#include "Thread.h"
#include <process.h>

unsigned int __stdcall Thread::ThreadProc(void* arg)
{
	auto thread = (Thread*)arg;


	if (thread->Init())
	{
		thread->Run();
	}

	thread->Shutdown();

	return 0;
}

Thread::Thread(int id, const std::wstring& name)
	: name_(name)
	, threadId_(id)
{
	hThread_ = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, &Thread::ThreadProc, this, CREATE_SUSPENDED, &osId_));

	std::atomic_init(&isRunning_, false);
}

Thread::~Thread()
{
	if (hThread_ != nullptr)
	{
		::CloseHandle(hThread_);
		hThread_ = nullptr;
	}
	
}

void Thread::Start()
{
	if (hThread_ != nullptr)
	{
		::ResumeThread(hThread_);
	}
}

void Thread::Terminate(unsigned int code)
{
	if (hThread_ != nullptr)
	{
		::TerminateThread(hThread_, code);
		::CloseHandle(hThread_);
		hThread_ = nullptr;
	}
}

void Thread::Shutdown()
{
	
}

bool Thread::Init()
{
	return true;
}

int Thread::GetId() const
{
	return threadId_;
}

const HANDLE Thread::GetHandle() const
{
	return hThread_;
}

bool Thread::Wait(DWORD timeout)
{
	if (hThread_ == nullptr)
	{
		return false;
	}

	if (::WaitForSingleObject(hThread_, timeout) != WAIT_OBJECT_0)
	{
		return false;
	}

	return true;
}

