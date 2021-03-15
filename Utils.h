/*
 * Utils.h
 *
 *  Created on: Nov 16, 2019
 *      Author: jeremy
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>

enum StatValType
{
	THREEPOINTFGMADE,
	TOTALFGMADE,
	TOTALFGATT,
	TURNOVER,
	OFFREB,
	TOTALREB,
	TOTALFTMADE,
	TOTALFTATT,
	TOTALPOSSESSIONS
};

enum BoxScoreFormatType {
	ASPX,
	XML,
	DBML,
	PDF,
	OTHER
};

class Utils
{
public:
	Utils() = delete;
	~Utils() = delete;
	Utils(const Utils &) = delete;
	static std::string exec(const std::string & cmd);
	static void Run(int startid);
};



#endif /* UTILS_H_ */
