/*
 * SportsRefBoxScore.h
 *
 *  Created on: Jan 18, 2022
 *      Author: jeremy
 */

#ifndef SPORTSREFBOXSCORE_H_
#define SPORTSREFBOXSCORE_H_

#include "IBoxScore.h"

class SportsRefBoxScore : public IBoxScore
{
public:
	std::optional<std::pair<Stats,Stats>> ProcessUrl(const std::string & url, const std::string & startdate);
};



#endif /* SPORTSREFBOXSCORE_H_ */
