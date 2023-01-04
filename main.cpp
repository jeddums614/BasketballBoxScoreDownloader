/*
 * main.cpp
 *
 *  Created on: Nov 2, 2019
 *      Author: jeremy
 */

#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <thread>
#include "Utils.h"
#include "ThreadPool.h"
#include "DbWrapper.h"

int main(int argc, char** argv)
{
	// number of threads
	unsigned int numThreads = std::thread::hardware_concurrency();
	if (numThreads == 0)
	{
		numThreads = sysconf(_SC_NPROCESSORS_ONLN);
	}

	std::string numTeamQuery = "select id from team";
	std::vector<std::vector<std::string>> numRes = DBWrapper::GetResults(numTeamQuery);

	ThreadPool tp;
	std::vector<std::future<void>> results;

	for (std::vector<std::string> res : numRes) {
		try {
			int i = std::stoi(res[0]);
			std::cout << "passing " << i << " as parameter" << std::endl;
		    results.emplace_back(tp.enqueue([i](){Utils::Run(i);}));
		}
		catch(...) {
			std::cout << "error with stoi" << std::endl;
		}
	}

	for (auto& res : results) {
		res.get();
	}

	return 0;
}
