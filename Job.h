#pragma once

template <int N>
struct Paramunpack
{
	template <typename ObjectType, typename... MemFuncArgs, typename ... TupleArgs, typename... Args>
	static void Execute(ObjectType* obj, void(ObjectType::*memFunc)(MemFuncArgs...), const std::tuple<TupleArgs...>& targs, Args&&... args)
	{
		Paramunpack<N - 1>::Execute(obj, memFunc, targs, std::get<N - 1>(targs), std::forward<Args>(args)...);
	}
};

template <>
struct Paramunpack<0>
{
	template <typename ObjectType, typename... MemFuncArgs, typename ... TupleArgs, typename... Args>
	static void Execute(ObjectType* obj, void(ObjectType::*memFunc)(MemFuncArgs...), const std::tuple<TupleArgs...>& targs, Args&&... args)
	{
		(obj->*memFunc)(std::forward<Args>(args)...);
	}
};

template <typename ObjectType, typename... MemFuncArgs, typename... TupleArgs>
void Execute(ObjectType* obj, void (ObjectType::*memFunc)(MemFuncArgs...), std::tuple<TupleArgs...> const& tupleArgs)
{
	Paramunpack<sizeof...(TupleArgs)>::Execute(obj, memFunc, tupleArgs);
}



struct Node
{
	Node()
		: next(nullptr)
	{}

	Node* volatile next;
};


struct Job
{
	Job() {}
	virtual ~Job() {}
	
	virtual void onExecute()
	{
	}

	Node	node;
};



template <typename ObjectType, typename... Args>
struct SomeJob : public Job
{
	using TupleArgs = std::tuple<Args...>;
	using MemFunc = void (ObjectType::*)(Args... args);

	ObjectType*		object_;
	TupleArgs		tups_;
	MemFunc			memberFunc_;

	SomeJob(ObjectType* obj, MemFunc memfunc, Args&& ... args)
		: object_(obj)
		, tups_(std::forward<Args>(args)...)
		, memberFunc_(memfunc)
	{

	}

	virtual ~SomeJob() { }

	void onExecute() override
	{
		::Execute(object_, memberFunc_, tups_);
	}

};