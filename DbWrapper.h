/*
 * DbWrapper.h
 *
 *  Created on: Dec 20, 2019
 *      Author: jeremy
 */

#ifndef DBWRAPPER_H_
#define DBWRAPPER_H_

#include<vector>
#include<string>

class DBWrapper
{
public:
	DBWrapper() = delete;
	DBWrapper(const DBWrapper &) = delete;
	~DBWrapper() = delete;
	DBWrapper& operator=(const DBWrapper &) = delete;
	static std::vector<std::vector<std::string>> GetResults(const std::string & query);
	static bool AddEntry(const std::string & query);
};


#endif /* DBWRAPPER_H_ */
