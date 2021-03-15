/*
 * ThreadPool.cpp
 *
 *  Created on: Mar 13, 2021
 *      Author: jeremy
 */

#include "ThreadPool.h"

ThreadPool::ThreadPool(int numThreads)
{
	shutdown.store(false, std::memory_order_relaxed);
	for (int i = 0; i < numThreads; ++i) {
		workerThreads.emplace_back(std::thread([this]()
		{
			while (true)
			{
				std::function<void()> job;

				{
					std::unique_lock<std::mutex> lock(queueMutex);
					notifier.wait(lock, [this] {return !jobQueue.empty() || shutdown.load(std::memory_order_relaxed); });

					if (shutdown.load(std::memory_order_relaxed))
					{
						break;
					}

					job = std::move(jobQueue.front());

					jobQueue.pop();
				}
				job();
			}
		}));
	}
}

ThreadPool::~ThreadPool()
{
	shutdown.store(true, std::memory_order_relaxed);
	notifier.notify_all();

	for (std::thread& th : workerThreads)
	{
		th.join();
	}
}


