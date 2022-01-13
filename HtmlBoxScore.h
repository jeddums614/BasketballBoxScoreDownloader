/*
 * HtmlBoxScore.h
 *
 *  Created on: Jan 11, 2022
 *      Author: jeremy
 */

#ifndef HTMLBOXSCORE_H_
#define HTMLBOXSCORE_H_

#include "IBoxScore.h"

class HtmlBoxScore : public IBoxScore
{
public:
	virtual ~HtmlBoxScore(){}
	std::optional<std::pair<Stats,Stats>> ProcessUrl(const std::string & url, const std::string & startdate);
};



#endif /* HTMLBOXSCORE_H_ */
