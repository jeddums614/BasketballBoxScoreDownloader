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

	std::string numTeamQuery = "select count(*) from team";
	std::vector<std::vector<std::string>> numRes = DBWrapper::GetResults(numTeamQuery);

	int numTeams = std::stoi(numRes[0][0]);

	ThreadPool tp;
	std::vector<std::future<void>> results;

	for (int i = 0; i < numTeams; ++i) {
		std::cout << "passing " << i+1 << " as parameter" << std::endl;
	    results.emplace_back(tp.enqueue([i](){Utils::Run(i+1);}));
	}

	for (auto& res : results) {
		res.get();
	}

	return 0;
}
