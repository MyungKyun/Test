#pragma once
#include "Thread.h"
#include "WorkerThread.h"
#include <array>
#include <vector>
#include <assert.h>

class ThreadManager
{
	enum
	{
		MAX_THREAD_COUNT = 64
	};
private:
	
	using ThreadList = std::array<Thread*, MAX_THREAD_COUNT>;
	ThreadList  threads_ = { nullptr, };
	std::atomic_uint32_t threadCount_ = 0;
	std::vector<HANDLE> threadHandles_;
public:
	~ThreadManager();

public:

	template <typename ThreadType,  typename ... Args>
	void ThreadStart(int count,  Args&&... args)
	{
		if (count <= 0)
		{
			SYSTEM_INFO si;
			::GetSystemInfo(&si);

			count = si.dwNumberOfProcessors;
		}

		for (int i = 0; i < count; ++i)
		{
			unsigned int id = threadCount_++;
			assert(id < MAX_THREAD_COUNT);

			ThreadType* thread = new ThreadType(id, std::forward<Args>(args)...);
			if (start(thread) == false)
			{
				threadCount_--;
				delete thread;
				return;
			}
		}
	}

	const Thread* GetThread(int id) const;
	void	GetHandles(HANDLE* handles);

	int		GetThreadCount() const { return threadCount_; }
private:

	bool start(WorkerThread* thread);
};

