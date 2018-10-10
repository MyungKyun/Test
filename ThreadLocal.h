#pragma once

#include <memory>

class JobExecuterQueue;
class Jobable;

extern __declspec(thread) JobExecuterQueue*		LExecuters;
extern __declspec(thread) int					LThreadId;
extern __declspec(thread) Jobable*				LIsCurrentThreadRunningJobExecuter;


void Initialize_TLS(int threadId);
void Terminate_TLS();
