/*
 * DbmlBoxScore.h
 *
 *  Created on: Nov 11, 2019
 *      Author: jeremy
 */

#ifndef DBMLBOXSCORE_H_
#define DBMLBOXSCORE_H_

#include "IBoxScore.h"

class DbmlBoxScore : public IBoxScore
{
public:
	virtual ~DbmlBoxScore(){}
	std::optional<std::pair<Stats,Stats>> ProcessUrl(std::string_view url, std::string_view startdate);
};



#endif /* DBMLBOXSCORE_H_ */
