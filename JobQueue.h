#pragma once
#include "Job.h"
#include <atomic>
#include <concurrent_queue.h>
#include <Windows.h>
#include <deque>
#include <assert.h>
#include <vector>
#include "ThreadLocal.h"
#include "ThreadManager.h"



class JobQueue
{

	Concurrency::concurrent_queue<std::shared_ptr<Job>> queue_;
	std::atomic_int		count_;
	std::vector<std::shared_ptr<Job>> allJob_;
public: 

	JobQueue()
	{
		std::atomic_init(&count_, 0);
	}
	
	void PushJob(std::shared_ptr<Job> val)
	{
		queue_.push(val);
		count_.fetch_add(1);
	}

	bool PopJob(std::shared_ptr<Job>& out)
	{

		
		if (!queue_.try_pop(out))
		{
			return false;
		}
		/*while (queue_.try_pop(out))
		{
			allJob_.push_back(out);
			count_.fetch_sub(1);
		}
*/
		count_.fetch_sub(1);
		return true;
		//return allJob_;
	}
};

class JobQueue2
{

private:
	std::atomic<Node*>	head_;
	Node*		   tail_;
	Node		   stub_;

	__int64		   offset_;
	
public :
	JobQueue2()
		: head_(&stub_)
		, tail_(&stub_)
	{
		offset_ = offsetof(struct Job, node);
		
	}

	~JobQueue2()
	{}

	void PushJob(Job* newJob)
	{
		Node*	prev = (Node*)std::atomic_exchange_explicit(&head_, &newJob->node, std::memory_order_acq_rel);
		prev->next = &(newJob->node);
	}

	Job* PopJob()
	{
		Node* tempTail = tail_;
		Node* next = tempTail->next;

		if (tempTail == &stub_)
		{
			if (nullptr == next)
			{
				return nullptr;
			}

			//Ã³À½ ²¨³¾¶§
			tail_ = next;
			tempTail = next;
			next = next->next;
		}

		if (next)
		{
			tail_ = next;

			return reinterpret_cast<Job*>(reinterpret_cast<__int64>(tempTail) - offset_);
		}

		Node* tempHead = head_;
		if (tempTail != tempHead)
		{
			return nullptr;
		}

		stub_.next = nullptr;
		Node* prev = (Node*)std::atomic_exchange_explicit(&head_, &stub_, std::memory_order_acq_rel);
		prev->next = &stub_;

		next = tempTail->next;
		if (next)
		{
			tail_ = next;

			return reinterpret_cast<Job*>((reinterpret_cast<__int64>(tempTail) - offset_));
		}

		return nullptr;
	}
};





class JobExecuterQueue
{
public:
	std::deque<std::shared_ptr<Jobable>> jobExecuters_;
};


class Jobable  : public std::enable_shared_from_this<Jobable>
{
	//JobQueue		 jobs_;
	JobQueue2		jobs_;
	std::atomic_int remainJobCount_;
	std::atomic_int refCount_;

protected:

	Jobable()
		: remainJobCount_(0)
		, refCount_(0)
	{}

public:

	

	virtual ~Jobable()
	{
		assert(refCount_ == 0);
	}

	template <typename ObjectType, typename... Args>
	void DoasyncJob(void(ObjectType::*memfunc)(Args...), Args&&... args)
	{
		decltype(auto) job = new SomeJob<ObjectType, Args...>(static_cast<ObjectType*>(this), memfunc, std::forward<Args>(args)...);
		doJob(job);
	}

	void IncreaseRefForThisObj()
	{
		refCount_.fetch_add(1);
	}

	void decreaseRefForThisObj()
	{
		refCount_.fetch_sub(1);
	}

private:

	void doJob(Job* job)
	{
		if (remainJobCount_.fetch_add(1) != 0)
		{
			jobs_.PushJob(job);
		}
		else
		{
			jobs_.PushJob(job);

			IncreaseRefForThisObj();

			if (LIsCurrentThreadRunningJobExecuter != nullptr)
			{
				LExecuters->jobExecuters_.push_back(shared_from_this());
			}
			else
			{
				LIsCurrentThreadRunningJobExecuter = this; 

				executeAllJob();

				while (!LExecuters->jobExecuters_.empty())
				{
					decltype(auto) executer = LExecuters->jobExecuters_.front();
					LExecuters->jobExecuters_.pop_front();
					executer->executeAllJob();
					executer->decreaseRefForThisObj();
				}

				LIsCurrentThreadRunningJobExecuter = nullptr;
				decreaseRefForThisObj();
				
			}
		}
	}


	void executeAllJob()
	{
	/*	std::shared_ptr<Job> popJob;
		while (jobs_.PopJob(popJob))
		{
			popJob->onExecute();

			if (remainJobCount_.fetch_sub(1) == 1)
				break;
		}*/

		while (1)
		{
			
			Job* job = jobs_.PopJob();
			if (job != nullptr)
			{
				job->onExecute();
				delete job;
				if (remainJobCount_.fetch_sub(1) == 1)
					break;
			}
		}
		
	}

};

