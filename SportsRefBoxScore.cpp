/*
 * SportsRefBoxScore.cpp
 *
 *  Created on: Jan 18, 2022
 *      Author: jeremy
 */

#include "SportsRefBoxScore.h"
#include "Downloader.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <regex>

std::optional<std::pair<Stats,Stats>> SportsRefBoxScore::ProcessUrl(const std::string & url, const std::string & startdate)
{
	std::string bscontent = Downloader::GetContent(url);
	std::istringstream iss{bscontent};
	std::string line;
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

	while (std::getline(iss, line)) {
		if (std::regex_search(line, tmp, std::regex("<title>([A-Za-z0-9&.\\-()'?_#\\/,\\[\\]; ]+) vs\\.* ([A-Za-z0-9&.\\-()'?_#\\/,\\[\\]; ]+) Box Score, (\\S+) (\\d{1,2}), (\\d{2,4}) \\|"))) {
			std::string team1 = tmp.str(1);
			std::string team2 = tmp.str(2);
			awaystatline["TEAM"] = team1;
			awayteam.SetTeamName(team1);

			homestatline["TEAM"] = team2;
			hometeam.SetTeamName(team2);

			std::string tmpmon = tmp.str(3);
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

			int day = std::stoi(tmp.str(4));
			std::string daystr = "";
			if (day < 10)
			{
				daystr += "0";
			}
			daystr += tmp.str(4);
			int year = std::stoi(tmp.str(5));
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
		else if (line.find("School Totals") != std::string::npos) {
			++totallabelcount;
			numvalsfound = 0;
			std::size_t strsz = std::strlen("School Totals");
			std::size_t totallabelpos = line.find("School Totals");

			std::string totalstatline = line.substr(totallabelpos+strsz);

			std::string::const_iterator scit(totalstatline.cbegin());
			std::regex totalvalregex(">(\\d{1,3})<");

			while (std::regex_search(scit,totalstatline.cend(),tmp2,totalvalregex)) {
				++numvalsfound;

				switch (numvalsfound) {
				case 2: // FGM
					if (totallabelcount == 1) {
						awaystatline["FGM"] = tmp2.str(1);
					}
					else if (totallabelcount == 3) {
						homestatline["FGM"] = tmp2.str(1);
					}
					break;

				case 3: // FGA
					if (totallabelcount == 1) {
						awaystatline["FGA"] = tmp2.str(1);
					}
					else if (totallabelcount == 3) {
						homestatline["FGA"] = tmp2.str(1);
					}
					break;
				case 6: // 3PT
					if (totallabelcount == 1) {
						awaystatline["3PT"] = tmp2.str(1);
					}
					else if (totallabelcount == 3) {
						homestatline["3PT"] = tmp2.str(1);
					}
					break;
				case 8: // FTM
					if (totallabelcount == 1) {
						awaystatline["FTM"] = tmp2.str(1);
					}
					else if (totallabelcount == 3) {
						homestatline["FTM"] = tmp2.str(1);
					}
					break;
				case 9: // FTA
					if (totallabelcount == 1) {
						awaystatline["FTA"] = tmp2.str(1);
					}
					else if (totallabelcount == 3) {
						homestatline["FTA"] = tmp2.str(1);
					}
					break;
				case 10: // OREB
					if (totallabelcount == 1) {
						awaystatline["OREB"] = tmp2.str(1);
					}
					else if (totallabelcount == 3) {
						homestatline["OREB"] = tmp2.str(1);
					}
					break;
				case 12: // REB
					if (totallabelcount == 1) {
						awaystatline["REB"] = tmp2.str(1);
					}
					else if (totallabelcount == 3) {
						homestatline["REB"] = tmp2.str(1);
					}
					break;
				case 16: // TO
					if (totallabelcount == 1) {
						awaystatline["TO"] = tmp2.str(1);
					}
					else if (totallabelcount == 3) {
						homestatline["TO"] = tmp2.str(1);
					}
					break;
				case 18: // PTS
					if (totallabelcount == 1) {
						awaystatline["PTS"] = tmp2.str(1);
					}
					else if (totallabelcount == 3) {
						homestatline["PTS"] = tmp2.str(1);
					}
					break;

				default:
					break;
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

