/*
 * BoxScoreType.h
 *
 *  Created on: Nov 11, 2019
 *      Author: jeremy
 */

#ifndef IBOXSCORE_H_
#define IBOXSCORE_H_

#include "Stats.h"
#include <optional>

class IBoxScore
{
public:
	virtual std::optional<std::pair<Stats, Stats>> ProcessUrl(const std::string & url, const std::string & startdate) = 0;
	virtual ~IBoxScore()
	{

	}
};



#endif /* IBOXSCORE_H_ */
