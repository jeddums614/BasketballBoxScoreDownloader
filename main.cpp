/*
 * main.cpp
 *
 *  Created on: Nov 2, 2019
 *      Author: jeremy
 */

#include <iostream>
#include <thread>
#include <vector>
#include "Utils.h"

int main(int argc, char** argv)
{
	unsigned int num = std::thread::hardware_concurrency();
	std::cout << "num threads = " << num << std::endl;

	std::vector<std::thread> thVector;

	unsigned int startid = 1;
	unsigned int endid = 59;
	for (unsigned int i = 0; i < num; ++i)
	{
		std::cout << "passing " << startid << " and " << endid << " as parameters" << std::endl;
		thVector.emplace_back(Utils::Run,startid,endid);
		startid += 59;
		endid += 59;
		if (endid > 353)
		{
			endid = 353;
		}
	}

	for (std::thread & th : thVector)
	{
		if (th.joinable())
		{
			th.join();
		}
	}

	return 0;
}
