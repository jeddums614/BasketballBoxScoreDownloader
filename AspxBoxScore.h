/*
 * AspxBoxScore.h
 *
 *  Created on: Nov 11, 2019
 *      Author: jeremy
 */

#ifndef ASPXBOXSCORE_H_
#define ASPXBOXSCORE_H_

#include "IBoxScore.h"

class AspxBoxScore : public IBoxScore
{
public:
	std::optional<std::pair<Stats,Stats>> ProcessUrl(const std::string & url, const std::string & startdate);
};



#endif /* ASPXBOXSCORE_H_ */
