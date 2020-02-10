/*
 * PdfBoxScore.h
 *
 *  Created on: Nov 12, 2019
 *      Author: jeremy
 */

#ifndef PDFBOXSCORE_H_
#define PDFBOXSCORE_H_

#include "IBoxScore.h"

class PdfBoxScore : public IBoxScore
{
public:
	std::optional<std::pair<Stats,Stats>> ProcessUrl(std::string_view url, std::string_view startdate);
};


#endif /* PDFBOXSCORE_H_ */
