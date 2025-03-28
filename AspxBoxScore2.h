/*
 * AspxBoxScore2.h
 *
 *  Created on: Feb 24, 2025
 *      Author: jeremy
 */

#ifndef ASPXBOXSCORE2_H_
#define ASPXBOXSCORE2_H_

#include "IBoxScore.h"

class AspxBoxScore2 : public IBoxScore
{
public:
	std::optional<std::pair<Stats,Stats>> ProcessUrl(const std::string & url, const std::string & startdate);
};



#endif /* ASPXBOXSCORE2_H_ */
