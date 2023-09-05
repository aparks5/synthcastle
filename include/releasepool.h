#pragma once

#include <chrono>
#include <thread>

// props to Timur Doumler for his CPPCon 2015 talk 

class ReleasePool
{
public:
	ReleasePool(size_t intervalMs);
	virtual ~ReleasePool() {};

	void release();
	void stop();
	template<typename T> void add(const std::shared_ptr<T>& object)
	{
		if (!object.get()) {
			return;
		}
		std::lock_guard<std::mutex> lock(m);
		pool.emplace_back(object);
	};


private:
	// this timer callback needs to be implemented
	// in the GUI thread
	void timerCallback();

	std::vector<std::shared_ptr<void>> pool;
	std::mutex m;
	std::thread m_thread;
	bool m_running;
	size_t m_interval;

};
