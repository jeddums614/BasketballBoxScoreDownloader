/*
 * AspxBoxScore2.cpp
 *
 *  Created on: Feb 24, 2025
 *      Author: jeremy
 */

#include "AspxBoxScore2.h"
#include "Downloader.h"
#include <iomanip>
#include <iostream>
#include <regex>
#include <iterator>
#include <optional>
#include <sstream>


std::optional<std::pair<Stats,Stats>> AspxBoxScore2::ProcessUrl(const std::string & url, const std::string & startdate)
{
	std::string bscontent = Downloader::GetContent(url);
	std::istringstream iss{bscontent};
	std::string line;
	int numhyphensfound = 0,numvalsfound = 0;
	std::string datestr = "";
	Stats awayteam, hometeam;
	std::map<std::string, std::string> awaystatline, homestatline;
	std::smatch teammatch, tmp, tmp2;

	if (std::regex_search(bscontent.cbegin(), bscontent.cend(),tmp,std::regex("([0-9]{1,2})\\/([0-9]{1,2})\\/([0-9]{2,4})")))
	{
		int year = std::stoi(tmp.str(3));
		if (year < 2000)
		{
		    year += 2000;
		}

		int month = std::stoi(tmp.str(1));

		datestr = std::to_string(year) + "-";
		if (month < 10)
		{
			datestr += "0";
		}
		datestr += std::to_string(month) + "-";

		int day = std::stoi(tmp.str(2));
		if (day < 10)
		{
			datestr += "0";
		}
		datestr += std::to_string(day);

		// Exhibition check
		//std::cout << datestr << std::endl;
		if (datestr.compare(startdate) < 0)
		{
			return std::nullopt;
		}
	}

	if (std::regex_search(bscontent.cbegin(),bscontent.cend(),tmp,std::regex("<h2[A-Za-z0-9.\\-\\!=\"\\[\\]_ ]+>[<!\\-\\[>]*([A-Za-z0-9.#\\/()'&;\\- ]+) [atvs]+ ([A-Za-z0-9.#\\/()'&;\\- ]+)<",std::regex_constants::icase|std::regex_constants::multiline)))
	{
		awayteam.SetTeamName(tmp[1]);
		hometeam.SetTeamName(tmp[2]);
		//std::cout << tmp[1] << "," << tmp[2] << std::endl;
	}

	//if <h2 class="s-common__header-title !s-text-heading-large text-theme-safe-light border-theme-brand-light !m-0 border-l-4 border-solid px-4 pl-[20px]">Fresno St. at UCLA</h2>
	std::string::const_iterator scit(bscontent.cbegin());
	while (std::regex_search(scit,bscontent.cend(),tmp,std::regex("<td class=\"!bg-theme-surface !text-theme-safe s-text-paragraph-small-bold\" data-v-c1ee6f7a[=\"]*>([0-9-]+)<",std::regex_constants::icase)))
	{
		std::string strval = tmp.str(1);
		//std::cout << strval << std::endl;
		std::smatch value;
		if (std::regex_search(strval,value,std::regex("(\\d{1,3})-(\\d{1,3})")))
		{
			++numhyphensfound;
			switch(numhyphensfound)
			{
			case 1:
				awaystatline["FGM"] = value.str(1);
				awaystatline["FGA"] = value.str(2);
				break;
			case 2:
				awaystatline["3PT"] = value.str(1);
				break;
			case 3:
				awaystatline["FTM"] = value.str(1);
				awaystatline["FTA"] = value.str(2);
				break;
			case 4:
				awaystatline["OREB"] = value.str(1);
				break;
			case 5:
				homestatline["FGM"] = value.str(1);
				homestatline["FGA"] = value.str(2);
				break;
			case 6:
				homestatline["3PT"] = value.str(1);
				break;
			case 7:
				homestatline["FTM"] = value.str(1);
				homestatline["FTA"] = value.str(2);
				break;
			case 8:
				homestatline["OREB"] = value.str(1);
				break;
			default:
				break;
			}
		}
		else
		{
			++numvalsfound;
			switch (numvalsfound)
			{
			case 1:
				awaystatline["MIN"] = strval;
				break;
			case 2:
				awaystatline["REB"] = strval;
				break;
			case 5:
				awaystatline["TO"] = strval;
				break;
			case 8:
				awaystatline["PTS"] = strval;
				break;
			case 9:
				homestatline["MIN"] = strval;
				break;
			case 10:
				homestatline["REB"] = strval;
				break;
			case 13:
				homestatline["TO"] = strval;
				break;
			case 16:
				homestatline["PTS"] = strval;
				break;
			default:
				break;
			}
		}
		scit = tmp.suffix().first;
	}

	if (!datestr.empty())
	{
		awayteam.SetDateString(datestr);
		awayteam.SetTeamPoints(std::stod(awaystatline["PTS"]));
		awayteam.SetThreePointFieldGoals(std::stod(awaystatline["3PT"]));
		awayteam.SetTotalFieldGoals(std::stod(awaystatline["FGM"]));
		awayteam.SetTotalFieldGoalAttempts(std::stod(awaystatline["FGA"]));
		awayteam.SetTotalTurnovers(std::stod(awaystatline["TO"]));
		awayteam.SetTotalOffensiveRebounds(std::stod(awaystatline["OREB"]));
		awayteam.SetTotalRebounds(std::stod(awaystatline["REB"]));
		awayteam.SetTotalFreeThrows(std::stod(awaystatline["FTM"]));
		awayteam.SetTotalFreeThrowAttempts(std::stod(awaystatline["FTA"]));
		awayteam.SetTotalMinutes(std::stod(awaystatline["MIN"]));

		hometeam.SetDateString(datestr);
		hometeam.SetTeamPoints(std::stod(homestatline["PTS"]));
		hometeam.SetThreePointFieldGoals(std::stod(homestatline["3PT"]));
		hometeam.SetTotalFieldGoals(std::stod(homestatline["FGM"]));
		hometeam.SetTotalFieldGoalAttempts(std::stod(homestatline["FGA"]));
		hometeam.SetTotalTurnovers(std::stod(homestatline["TO"]));
		hometeam.SetTotalOffensiveRebounds(std::stod(homestatline["OREB"]));
		hometeam.SetTotalRebounds(std::stod(homestatline["REB"]));
		hometeam.SetTotalFreeThrows(std::stod(homestatline["FTM"]));
		hometeam.SetTotalFreeThrowAttempts(std::stod(homestatline["FTA"]));
		hometeam.SetTotalMinutes(std::stod(homestatline["MIN"]));

		return std::make_pair(awayteam,hometeam);
	}
	else
	{
		return std::nullopt;
	}
}
