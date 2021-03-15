/*
 * ThreadPool.h
 *
 *  Created on: Mar 13, 2021
 *      Author: jeremy
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <future>
#include <type_traits>

class ThreadPool {
public:
	ThreadPool(int numThreads = std::thread::hardware_concurrency());
	~ThreadPool();
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool &&) = delete;

	ThreadPool & operator=(const ThreadPool &) = delete;
	ThreadPool & operator=(ThreadPool &&) = delete;

	//add any arg # function to queue
	template <typename Func, typename... Args>
	auto enqueue(Func&& f, Args&&... args);

private:
	std::vector<std::thread> workerThreads;
	std::mutex queueMutex;
	std::atomic<bool> shutdown;
	std::queue<std::function<void()>> jobQueue;
	std::condition_variable notifier;
};

template <typename Func, typename... Args>
auto ThreadPool::enqueue(Func&& f, Args&&... args)
{
	//get return type of the function
	using RetType = typename std::invoke_result<Func, Args...>::type;   // C++17
	//using RetType = typename std::result_of<Func(Args...)>::type; // C++11, C++14

	// C++17
	//auto task = std::make_shared<std::packaged_task<RetType()>>([&f, &args...]() { return f(std::forward<Args>(args)...); });

	// C++11, C++14
	auto task = std::make_shared< std::packaged_task<RetType()> >(
            std::bind(std::forward<Func>(f), std::forward<Args>(args)...)
        );

	{
		// lock jobQueue mutex, add job to the job queue
		std::unique_lock<std::mutex> lock(queueMutex);

		//place the job into the queue
		jobQueue.emplace([task]() {
			(*task)();
		});
	}
	notifier.notify_one();

	return task->get_future();
}

#endif /* THREADPOOL_H_ */
