#include "ThreadManager.h"


ThreadManager::~ThreadManager()
{
	for (size_t i = 0; i < threads_.size(); ++i)
	{
		Thread* thread = threads_[i];

		if (nullptr != thread)
		{
			delete thread;
		}
	}
}

bool ThreadManager::start(WorkerThread * thread)
{
	if (thread == nullptr || thread->GetHandle() == nullptr)
	{
		return false;
	}

	unsigned int id = thread->GetId();
	if (id < 0 || id >= threads_.size())
	{
		return false;
	}

	if( threads_[id] != nullptr )
	{
		return false;
	}

	thread->Start();

	threads_[id] = thread;
	threadHandles_.push_back(thread->GetHandle());
		
	return true;
}

const Thread* ThreadManager::GetThread(int id) const
{
	if (id < 0 || id >= threads_.size())
	{
		return nullptr;
	}

	return threads_[id];
}

void	ThreadManager::GetHandles(HANDLE* handles)
{
	int idx = 0;
	for (auto& i : threadHandles_)
	{
		handles[idx++] = i;
	}
}