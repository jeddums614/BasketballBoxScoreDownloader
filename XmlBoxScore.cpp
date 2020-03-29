/*
 * XmlBoxScore.cpp
 *
 *  Created on: Nov 11, 2019
 *      Author: jeremy
 */

#include "XmlBoxScore.h"
#include "Downloader.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <regex>

std::optional<std::pair<Stats,Stats>> XmlBoxScore::ProcessUrl(const std::string & url, const std::string & startdate)
{
	std::string bscontent = Downloader::GetContent(url);
	std::istringstream iss{bscontent};
	std::string line;
	bool totalfound = false;
	int totallabelcount = 0;
	std::string team1 = "";
	std::string team2 = "";
	std::string datestr = "";
	std::map<std::string, std::string> awaystatline;
	std::map<std::string, std::string> homestatline;
	std::smatch teammatch;
	std::smatch tmp,tmp2;
	Stats awayteam, hometeam;
	int numvalsfound = 0;
	int numhyphensfound = 0;

	while (std::getline(iss,line))
	{
		if (line.find("Date/Time") != std::string::npos)
		{
			std::smatch datematch;
			if (std::regex_search(line,datematch,std::regex(">(\\S+) (\\d{1,2}), (\\d{2,4})")))
			{
				std::string tmpmon = datematch.str(1);
				std::transform(tmpmon.begin(),tmpmon.end(),tmpmon.begin(),::tolower);

				std::string monthstr = "";
				if (tmpmon.compare("january") == 0)
				{
					monthstr = "01";
				}
				else if (tmpmon.compare("february") == 0)
				{
					monthstr = "02";
				}
				else if (tmpmon.compare("march") == 0)
				{
					monthstr = "03";
				}
				else if (tmpmon.compare("april") == 0)
				{
					monthstr = "04";
				}
				else if (tmpmon.compare("may") == 0)
				{
					monthstr = "05";
				}
				else if (tmpmon.compare("june") == 0)
				{
					monthstr = "06";
				}
				else if (tmpmon.compare("july") == 0)
				{
					monthstr = "07";
				}
				else if (tmpmon.compare("august") == 0)
				{
					monthstr = "08";
				}
				else if (tmpmon.compare("september") == 0)
				{
					monthstr = "09";
				}
				else if (tmpmon.compare("october") == 0)
				{
					monthstr = "10";
				}
				else if (tmpmon.compare("november") == 0)
				{
					monthstr = "11";
				}
				else if (tmpmon.compare("december") == 0)
				{
					monthstr = "12";
				}
				else
				{
					std::cout << "invalid month" << std::endl;
					std::exit(-111);
				}

				int day = std::stoi(datematch.str(2));
				std::string daystr = "";
				if (day < 10)
				{
					daystr += "0";
				}
				daystr += datematch.str(2);
				int year = std::stoi(datematch.str(3));
				if (year < 2000)
				{
					year += 2000;
				}

				datestr = std::to_string(year) + "-" + monthstr + "-" + daystr;
				// Exhibition check
				if (datestr.compare(startdate) < 0)
				{
					datestr = "";
					break;
				}
			}
		}
		else if (std::regex_search(line,tmp,std::regex(">Totals<",std::regex_constants::icase)))
		{
			totalfound = true;
			++totallabelcount;
		}
		else if (std::regex_search(line,tmp,std::regex("<\\/tr>")) && totalfound)
		{
			totalfound = false;
			numvalsfound = 0;
			numhyphensfound = 0;
		}
		else if (std::regex_search(line,tmp,std::regex("<div class=\"tab-label\" id=\"team-label-boxscore-v\">([A-Za-z0-9&.\\-()'?_#\\/,\\[\\]; ]+)<\\/div>")) && team1.empty())
		{
			team1 = tmp.str(1);
			awaystatline["TEAM"] = team1;
			awayteam.SetTeamName(team1);
		}
		else if (std::regex_search(line,tmp,std::regex("<div class=\"tab-label\" id=\"team-label-boxscore-h\">([A-Za-z0-9&.\\-()'?_#\\/,\\[\\]; ]+)<\\/div>")) && team2.empty())
		{
			team2 = tmp.str(1);
			homestatline["TEAM"] = team2;
			hometeam.SetTeamName(team2);
		}
		else if (std::regex_search(line,tmp,std::regex("(\\d{1,2})-(\\d{1,2})")) && totalfound)
		{
			++numhyphensfound;
			switch (numhyphensfound)
			{
			case 1:
				// FGM/FGA
				if (totallabelcount == 1)
				{
					awaystatline["FGM"] = tmp.str(1);
					awaystatline["FGA"] = tmp.str(2);
				}
				else if (totallabelcount == 2)
				{
					homestatline["FGM"] = tmp.str(1);
					homestatline["FGA"] = tmp.str(2);
				}
				break;

			case 2:
				// 3PT/3PTA
				if (totallabelcount == 1)
				{
					awaystatline["3PT"] = tmp.str(1);
				}
				else if (totallabelcount == 2)
				{
					homestatline["3PT"] = tmp.str(1);
				}
				break;

			case 3:
				// FTM/FTA
				if (totallabelcount == 1)
				{
					awaystatline["FTM"] = tmp.str(1);
					awaystatline["FTA"] = tmp.str(2);
				}
				else if (totallabelcount == 2)
				{
					homestatline["FTM"] = tmp.str(1);
					homestatline["FTA"] = tmp.str(2);
				}
				break;

			default:
				break;
			}
		}
		else if (std::regex_search(line,tmp,std::regex("(\\d{1,3})")) &&
				!std::regex_search(line,tmp2,std::regex("[.%]")) &&
				totalfound)
		{
			++numvalsfound;
			switch (numvalsfound)
			{
			case 1:
				if (totallabelcount == 1)
				{
					awaystatline["OREB"] = tmp.str(1);
				}
				else if (totallabelcount == 2)
				{
					homestatline["OREB"] = tmp.str(1);
				}
				break;
			case 3:
				if (totallabelcount == 1)
				{
					awaystatline["REB"] = tmp.str(1);
				}
				else if (totallabelcount == 2)
				{
					homestatline["REB"] = tmp.str(1);
				}
				break;
			case 7:
				if (totallabelcount == 1)
				{
					awaystatline["TO"] = tmp.str(1);
				}
				else if (totallabelcount == 2)
				{
					homestatline["TO"] = tmp.str(1);
				}
				break;
			case 9:
				if (totallabelcount == 1)
				{
					awaystatline["PTS"] = tmp.str(1);
				}
				else if (totallabelcount == 2)
				{
					homestatline["PTS"] = tmp.str(1);
				}
				break;
			default:
				break;
			}
		}
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
		return std::make_pair(awayteam,hometeam);
	}
	else
	{
		return std::nullopt;
	}
}

