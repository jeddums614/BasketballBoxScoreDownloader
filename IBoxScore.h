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
#include <string_view>

class IBoxScore
{
public:
	virtual std::optional<std::pair<Stats, Stats>> ProcessUrl(std::string_view url, std::string_view startdate) = 0;
	virtual ~IBoxScore()
	{

	}
};



#endif /* IBOXSCORE_H_ */
