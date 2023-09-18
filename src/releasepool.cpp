#pragma once

#include "releasepool.h"

ReleasePool::ReleasePool(size_t intervalMs)
	: m_running(false)
	, m_interval(intervalMs)
{
}


void ReleasePool::release()
{
	std::lock_guard<std::mutex> lock(m);
	pool.erase(std::remove_if(pool.begin(), pool.end(),
			  [](auto& object) { return object.use_count() <= 1; }),
				  pool.end());

	//auto delta = std::chrono::steady_clock::now() + std::chrono::milliseconds(m_interval);
	//std::this_thread::sleep_until(delta);
	//m_thread.detach(); 
}


void ReleasePool::stop() {
	//m_running = false;
	//m_thread.~thread();
}

