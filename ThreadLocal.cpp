#include "ThreadLocal.h"
#include "JobQueue.h"

__declspec(thread)	JobExecuterQueue*	LExecuters = nullptr;
__declspec(thread)	int					LThreadId = -1;
__declspec(thread) Jobable*				LIsCurrentThreadRunningJobExecuter = nullptr;


void Initialize_TLS(int threadId)
{
	LThreadId = threadId;
	LExecuters = new JobExecuterQueue;

}

void Terminate_TLS()
{
	LThreadId = -1;
}
