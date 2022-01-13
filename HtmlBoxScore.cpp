/*
 * HtmlBoxScore.cpp
 *
 *  Created on: Jan 11, 2022
 *      Author: jeremy
 */


#include "HtmlBoxScore.h"
#include "Downloader.h"
#include <regex>
#include <iostream>

std::optional<std::pair<Stats, Stats>> HtmlBoxScore::ProcessUrl(const std::string & url, const std::string & startdate)
{
	std::string bscontent = Downloader::GetContent(url);
	std::istringstream iss{bscontent};
	std::string line;
	std::string datestr = "";
	std::smatch tmp, tmp2;
	Stats awayteam, hometeam;
	std::map<std::string, std::string> awaystatline, homestatline;
	int totallabelcount = 0;
	int numvalsfound = 0;
	int numhyphensfound = 0;

	while (std::getline(iss, line)) {
		std::smatch titlematch;

		if (std::regex_search(line, tmp, std::regex("<title>Men's Basketball \\| ([A-Za-z0-9&.\\-()'?_#\\/,\\[\\]; ]+) vs\\.* ([A-Za-z0-9&.\\-()'?_#\\/,\\[\\]; ]+) \\| (\\d{1,2})\\/(\\d{1,2})\\/(\\d{2,4})<\\/title>"))) {
			std::string team1 = tmp.str(1);
			std::string team2 = tmp.str(2);
			awaystatline["TEAM"] = team1;
			awayteam.SetTeamName(team1);

			homestatline["TEAM"] = team2;
			hometeam.SetTeamName(team2);

			int year = std::stoi(tmp.str(5));
			if (year < 2000)
			{
				year += 2000;
			}

			datestr = std::to_string(year) + "-" + tmp.str(3) + "-" + tmp.str(4);
			// Exhibition check
			if (datestr.compare(startdate) < 0)
			{
				datestr = "";
				break;
			}
		}
		else if (line.find(">TOTALS<") != std::string::npos) {
			++totallabelcount;
			numhyphensfound = 0;
			numvalsfound = 0;
			std::size_t totallabelpos = line.find(">TOTALS<");

			std::string totalstatline = line.substr(totallabelpos+7);
			std::string::const_iterator scit(totalstatline.cbegin());
			std::regex totalvalregex(">(\\d{1,2})-(\\d{1,2})<|>(\\d{1,3})<");

			while (std::regex_search(scit,totalstatline.cend(),tmp2,totalvalregex)) {

				std::string val1 = tmp2.str(1);
				std::string val2 = tmp2.str(2);
				std::string val3 = tmp2.str(3);

				if (!val1.empty()) {
					++numhyphensfound;

					switch (numhyphensfound) {
					case 1:
						// FGM/FGA
						if (totallabelcount == 1)
						{
							awaystatline["FGM"] = val1;
							awaystatline["FGA"] = val2;
						}
						else if (totallabelcount == 2)
						{
							homestatline["FGM"] = val1;
							homestatline["FGA"] = val2;
						}
						break;
					case 2:
						// 3PT/3PTA
						if (totallabelcount == 1)
						{
							awaystatline["3PT"] = val1;
						}
						else if (totallabelcount == 2)
						{
							homestatline["3PT"] = val1;
						}
						break;
					case 3:
						// FTM/FTA
						if (totallabelcount == 1)
						{
							awaystatline["FTM"] = val1;
							awaystatline["FTA"] = val2;
						}
						else if (totallabelcount == 2)
						{
							homestatline["FTM"] = val1;
							homestatline["FTA"] = val2;
						}
						break;
					default:
						break;
					}
				}
				else if (!val3.empty()) {
					++numvalsfound;

					switch (numvalsfound) {
					case 1:
						if (totallabelcount == 1)
						{
							awaystatline["OREB"] = val3;
						}
						else if (totallabelcount == 2)
						{
							homestatline["OREB"] = val3;
						}
						break;
					case 3:
						if (totallabelcount == 1)
						{
							awaystatline["REB"] = val3;
						}
						else if (totallabelcount == 2)
						{
							homestatline["REB"] = val3;
						}
						break;
					case 7:
						if (totallabelcount == 1)
						{
							awaystatline["TO"] = val3;
						}
						else if (totallabelcount == 2)
						{
							homestatline["TO"] = val3;
						}
						break;
					case 9:
						if (totallabelcount == 1)
						{
							awaystatline["PTS"] = val3;
						}
						else if (totallabelcount == 2)
						{
							homestatline["PTS"] = val3;
						}
						break;
					default:
						break;
					}
				}

				scit = tmp2.suffix().first;
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
