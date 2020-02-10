/*
 * PdfBoxScore.cpp
 *
 *  Created on: Nov 12, 2019
 *      Author: jeremy
 */

#include "PdfBoxScore.h"
#include "Utils.h"
#include <iostream>
#include <regex>

std::optional<std::pair<Stats,Stats>> PdfBoxScore::ProcessUrl(std::string_view url, std::string_view startdate)
{
	// Download pdf
	std::stringstream downloadcommand;
	downloadcommand << "curl -s -L -o tmp.pdf " << url;
	//std::cout << downloadcommand.str() << std::endl;
	std::string tmp = Utils::exec(downloadcommand.str());
	// Convert pdf to text
	std::string pdftextcommand = "pdftotext -f 1 -l 1 -layout tmp.pdf -";
	//std::cout << pdftextcommand << std::endl;
	std::string bscontent = Utils::exec(pdftextcommand);
	std::stringstream iss(bscontent);
	std::string team1 = "";
	std::string team2 = "";
	std::string line;
	int totallabelcount = 0;
	std::string datestr = "";
	std::map<std::string, std::string> awaystatline;
	std::map<std::string, std::string> homestatline;
	Stats hometeam,awayteam;
	std::smatch teammatch;
	std::smatch tmp2;
	int numvalsfound = 0;
	while (std::getline(iss,line))
	{
		//std::cout << line << std::endl;
		std::smatch match;
		if (std::regex_search(line,match,std::regex("(\\d{1,2})\\/(\\d{1,2})\\/(\\d{2,4})")))
		{
			std::string monthstr = match.str(1);
			std::string daystr = match.str(2);
			int yearval = std::stoi(match.str(3));

			if (yearval < 2000)
			{
				yearval += 2000;
			}
			std::string yearstr = std::to_string(yearval);

			datestr = yearstr + "-" + match.str(1) + "-" + match.str(2);
			// Exhibition check
			if (datestr.compare("2019-11-05") < 0)
			{
				datestr = "";
				break;
			}
			//std::cout << datestr << std::endl;
		}
		else if ((std::regex_search(line,match,std::regex("^\\s+([A-Za-z0-9&.\\-()'?_#\\/,\\[\\]; ]+) at ([A-Za-z0-9&.\\-()'?_#\\/,\\[\\]; ]+)")) ||
				  std::regex_search(line,match,std::regex("^\\s+([A-Za-z0-9&.\\-()'?_#\\/,\\[\\]; ]+) vs ([A-Za-z0-9&.\\-()'?_#\\/,\\[\\]; ]+)")))&&
				team1.empty() && team2.empty())
		{
			team1 = match.str(1);
			awaystatline["TEAM"] = team1;
			awayteam.SetTeamName(team1);
			team2 = match.str(2);
			hometeam.SetTeamName(team2);
			if (std::regex_search(team2,match,std::regex("([A-Za-z0-9&.\\-()'?_#\\/,\\[\\] ]+)\\s{2,}")))
			{
				team2 = match.str(1);
				team2.erase(std::find_if(team2.rbegin(), team2.rend(), [](int ch) {
				        return !std::isspace(ch);
				    }).base(), team2.end());
				homestatline["TEAM"] = team2;
				hometeam.SetTeamName(team2);
			}
			//std::cout << "\"" << team1 << "\"" << "," << "\"" << team2 << "\"" << std::endl;
		}
		else if (std::regex_search(line,match,std::regex("Totals",std::regex_constants::icase)) && !team1.empty() && !team2.empty())
		{
			++totallabelcount;
			numvalsfound = 0;
			std::regex splitregex("\\s+|\\-");
			std::vector<std::string> statparts{std::sregex_token_iterator(line.begin(), line.end(), splitregex, -1),
				  std::sregex_token_iterator()};
			std::copy(statparts.begin(),statparts.end(),std::ostream_iterator<std::string>(std::cout,","));
			std::cout << std::endl;
			std::cout << statparts.size() << std::endl;

			for (std::string stat : statparts)
			{
				if (std::regex_search(stat,match,std::regex("(\\d{1,3})")))
				{
					++numvalsfound;
					switch (numvalsfound)
					{
					case 1:
						if (totallabelcount == 1)
						{
							awaystatline["FGM"] = match.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["FGM"] = match.str(1);
						}
						break;

					case 2:
						if (totallabelcount == 1)
						{
							awaystatline["FGA"] = match.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["FGA"] = match.str(1);
						}
						break;

					case 3:
						if (totallabelcount == 1)
						{
							awaystatline["3PT"] = match.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["3PT"] = match.str(1);
						}
						break;

					case 5:
						if (totallabelcount == 1)
						{
							awaystatline["FTM"] = match.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["FTM"] = match.str(1);
						}
						break;

					case 6:
						if (totallabelcount == 1)
						{
							awaystatline["FTA"] = match.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["FTA"] = match.str(1);
						}
						break;

					case 7:
						if (totallabelcount == 1)
						{
							awaystatline["OREB"] = match.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["OREB"] = match.str(1);
						}
						break;

					case 9:
						if (totallabelcount == 1)
						{
							awaystatline["REB"] = match.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["REB"] = match.str(1);
						}
						break;

					case 11:
						if (statparts.size() == 17)
						{
							if (totallabelcount == 1)
							{
							    awaystatline["PTS"] = match.str(1);
							}
							else if (totallabelcount == 2)
							{
							    homestatline["PTS"] = match.str(1);
							}
						}
						break;

					case 12:
						//if (statparts.size() == 18 || statparts.size() == 19 || statparts.size() == 20 || statparts.size() == 23 || statparts.size() == 24)
						if (statparts.size() > 17)
						{
							if (totallabelcount == 1)
							{
							    awaystatline["PTS"] = match.str(1);
							}
							else if (totallabelcount == 2)
							{
							    homestatline["PTS"] = match.str(1);
							}
						}
						break;

					case 13:
						if (statparts.size() == 17)
						{
							if (totallabelcount == 1)
							{
								awaystatline["TO"] = match.str(1);
							}
							else if (totallabelcount == 2)
							{
								homestatline["TO"] = match.str(1);
							}
						}
						break;
					case 14:
						//if (statparts.size() == 18 || statparts.size() == 19 || statparts.size() == 20 || statparts.size() == 23 || statparts.size() == 24)
						if (statparts.size() > 17)
						{
							if (totallabelcount == 1)
							{
							    awaystatline["TO"] = match.str(1);
							}
							else if (totallabelcount == 2)
							{
							    homestatline["TO"] = match.str(1);
							}
						}
						break;
					}
					//std::cout << match.str(1) << std::endl;
				}
			}
		}
		else if (std::regex_search(line,match,std::regex("(\\d{1,2})\\/(\\d{1,2})\\/(\\d{2,4})")))
		{
			int year = std::stoi(match.str(3));
			if (year < 2000)
			{
				year += 2000;
			}

			datestr = std::to_string(year) + "-" + match.str(1) + "-" + match.str(2);

			if (datestr.compare(startdate) < 0)
			{
				datestr = "";
				break;
			}
		}
	}

	std::remove("tmp.pdf");

	if (!datestr.empty() && awaystatline.size() > 0 && homestatline.size() > 0)
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
