/*
 * XmlBoxScore.h
 *
 *  Created on: Nov 11, 2019
 *      Author: jeremy
 */

#ifndef XMLBOXSCORE_H_
#define XMLBOXSCORE_H_

#include "IBoxScore.h"

class XmlBoxScore : public IBoxScore
{
public:
	std::optional<std::pair<Stats,Stats>> ProcessUrl(std::string_view url, std::string_view startdate);
};



#endif /* XMLBOXSCORE_H_ */
