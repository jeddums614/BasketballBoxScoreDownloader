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
	std::optional<std::pair<Stats,Stats>> ProcessUrl(const std::string & url, const std::string & startdate);
};



#endif /* XMLBOXSCORE_H_ */
