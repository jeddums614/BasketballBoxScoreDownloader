/*
 * AspxBoxScore.cpp
 *
 *  Created on: Nov 11, 2019
 *      Author: jeremy
 */

#include "AspxBoxScore.h"
#include "Downloader.h"
#include <iomanip>
#include <iostream>
#include <regex>
#include <optional>

std::optional<std::pair<Stats,Stats>> AspxBoxScore::ProcessUrl(const std::string & url, const std::string & startdate)
{
	std::string bscontent = Downloader::GetContent(url);
	std::istringstream iss{bscontent};
	std::string line;
	bool totalfound = false;
	int totallabelcount = 0;
	bool datelabelfound = false;
	std::string datestr = "";
	Stats awayteam, hometeam;
	std::map<std::string, std::string> awaystatline, homestatline;
	std::smatch teammatch, tmp, tmp2;

	while (std::getline(iss,line))
	{
		if (line.find(">Date<") != std::string::npos)
		{
			datelabelfound = true;
		}
		else if (datelabelfound)
		{
			std::smatch datematch;
			if (std::regex_search(line,datematch,std::regex(">(\\d{1,2})\\/(\\d{1,2})\\/(\\d{2,4})<")))
			{
				int year = std::stoi(datematch.str(3));
				if (year < 2000)
				{
					year += 2000;
				}

				datestr = std::to_string(year) + "-" + datematch.str(1) + "-" + datematch.str(2);
				// Exhibition check
				if (datestr.compare(startdate) < 0)
				{
					datestr = "";
					break;
				}
				else
				{
					awayteam.SetDateString(datestr);
					hometeam.SetDateString(datestr);
				}
				//std::cout << datestr << std::endl;
			}
			datelabelfound = false;
		}
		else if (std::regex_search(line,teammatch,std::regex("<h[1-6] class=\"sub-heading\">([A-Za-z0-9&.\\-();'?_#\\/,\\[\\] ]+)<\\/h[1-6]>")) && (awayteam.GetTeamName().empty() || hometeam.GetTeamName().empty()))
		{
			//if (team1.empty())
			if (awayteam.GetTeamName().empty())
			{
				std::string tmpname = teammatch.str(1).substr(0, teammatch.str(1).rfind(" "));
				awayteam.SetTeamName(tmpname);
			}
			else if (hometeam.GetTeamName().empty())
			{
				std::string tmpname = teammatch.str(1).substr(0, teammatch.str(1).rfind(" "));
				hometeam.SetTeamName(tmpname);
			}
		}
		else if (std::regex_search(line,tmp,std::regex(">Totals<",std::regex_constants::icase)) || std::regex_search(line,tmp,std::regex("<tr class=\"totals\">Totals<",std::regex_constants::icase)))
		{
			totalfound = true;
			++totallabelcount;
		}
		else if (std::regex_search(line,tmp,std::regex("<\\/tr>")) && totalfound == true)
		{
			totalfound = false;
		}
		else if (totalfound == true && (totallabelcount == 1 || totallabelcount == 2))
		{
			// 1 = away team stats, 2 = home team stats
			if (std::regex_search(line, tmp, std::regex("data-label=\"([A-Za-z-0-9]+)\">([0-9-]+)<\\/td>")))
			{
				std::size_t pos = tmp.str(2).find("-");
				if (pos != std::string::npos)
				{
					std::size_t pos = tmp.str(2).find("-");
					std::string value1 = tmp.str(2).substr(0,pos);
					std::string value2 = tmp.str(2).substr(pos+1);
					if (tmp.str(1).compare("FG") == 0 ||tmp.str(1).compare("FT") == 0)
					{
						if (totallabelcount == 1)
						{
							awaystatline[tmp.str(1)+"M"] = value1;
							awaystatline[tmp.str(1)+"A"] = value2;
						}
						else if (totallabelcount == 2)
						{
							homestatline[tmp.str(1)+"M"] = value1;
							homestatline[tmp.str(1)+"A"] = value2;
						}
					}
					else if (tmp.str(1).compare("3PT") == 0)
					{
						if (totallabelcount == 1)
						{
							awaystatline[tmp.str(1)] = value1;
						}
						else if (totallabelcount == 2)
						{
							homestatline[tmp.str(1)] = value1;
						}
					}
					else
					{
						if (totallabelcount == 1)
						{
							awaystatline["OREB"] = value1;
						}
						else if (totallabelcount == 2)
						{
							homestatline["OREB"] = value1;
						}
					}
				}
				else
				{
					if (totallabelcount == 1)
					{
						awaystatline[tmp.str(1)] = tmp.str(2);
					}
					else if (totallabelcount == 2)
					{
						homestatline[tmp.str(1)] = tmp.str(2);
					}
				}
			}
	    }
	}

	if (!datestr.empty())
	{
		awayteam.SetTeamPoints(std::stod(awaystatline["PTS"]));
		awayteam.SetThreePointFieldGoals(std::stod(awaystatline["3PT"]));
		awayteam.SetTotalFieldGoals(std::stod(awaystatline["FGM"]));
		awayteam.SetTotalFieldGoalAttempts(std::stod(awaystatline["FGA"]));
		awayteam.SetTotalTurnovers(std::stod(awaystatline["TO"]));
		awayteam.SetTotalOffensiveRebounds(std::stod(awaystatline["OREB"]));
		awayteam.SetTotalRebounds(std::stod(awaystatline["REB"]));
		awayteam.SetTotalFreeThrows(std::stod(awaystatline["FTM"]));
		awayteam.SetTotalFreeThrowAttempts(std::stod(awaystatline["FTA"]));

		hometeam.SetTeamPoints(std::stod(homestatline["PTS"]));
		hometeam.SetThreePointFieldGoals(std::stod(homestatline["3PT"]));
		hometeam.SetTotalFieldGoals(std::stod(homestatline["FGM"]));
		hometeam.SetTotalFieldGoalAttempts(std::stod(homestatline["FGA"]));
		hometeam.SetTotalTurnovers(std::stod(homestatline["TO"]));
		hometeam.SetTotalOffensiveRebounds(std::stod(homestatline["OREB"]));
		hometeam.SetTotalRebounds(std::stod(homestatline["REB"]));
		hometeam.SetTotalFreeThrows(std::stod(homestatline["FTM"]));
		hometeam.SetTotalFreeThrowAttempts(std::stod(homestatline["FTA"]));
		return std::make_pair(awayteam,hometeam);
	}
	else
	{
		return std::nullopt;
	}
}


