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
	// number of threads
	unsigned int num = 51;

	std::vector<std::thread> thVector;

	unsigned int startid = 1;
	unsigned int endid = 7;
	for (unsigned int i = 0; i < num; ++i, startid +=7, endid += 7)
	{
		if (endid > 353)
		{
			endid = 353;
		}
		std::cout << "passing " << startid << " and " << endid << " as parameters" << std::endl;
		thVector.emplace_back(Utils::Run,startid,endid);
	}

	try
	{
		for (std::thread & th : thVector)
		{
			if (th.joinable())
			{
				th.join();
			}
		}
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}
