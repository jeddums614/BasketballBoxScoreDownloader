/*
 * DbmlBoxScore.cpp
 *
 *  Created on: Nov 11, 2019
 *      Author: jeremy
 */

#include "DbmlBoxScore.h"
#include "Downloader.h"
#include "Utils.h"
#include <map>
#include <sstream>
#include <regex>
#include <iomanip>
#include <iostream>
#include <iterator>

std::optional<std::pair<Stats,Stats>> DbmlBoxScore::ProcessUrl(const std::string & url, const std::string & startdate)
{
	std::string bscontent = Downloader::GetContent(url);
	//std::cout << bscontent << std::endl;
	std::istringstream iss{bscontent};
	std::string line;
	bool totalfound = false;
	int totallabelcount = 0;
	std::string team1 = "";
	std::string team2 = "";
	std::string datestr = "";
	std::map<std::string, std::string> awaystatline;
	std::map<std::string, std::string> homestatline;
	Stats awayteam, hometeam;
	std::smatch teammatch;
	std::smatch tmp,tmp2;
	int numvalsfound = 0;
	int numhyphensfound = 0;
	while (std::getline(iss,line))
	{
		if ((std::regex_search(line,tmp2,std::regex("(\\d{1,2})\\.(\\d{1,2})\\.(\\d{2,4})[ ]+(Noon|[0-9:]+)",std::regex_constants::icase)) ||
			 std::regex_search(line,tmp2,std::regex("(\\d{1,2})\\/(\\d{1,2})\\/(\\d{2,4})[ ]+(Noon|[0-9:]+)",std::regex_constants::icase)) ||
			 std::regex_search(line,tmp2,std::regex("(\\d{1,2})-[ ]*(\\d{1,2})-(\\d{2,4})[ ]+(Noon|[0-9:]+)",std::regex_constants::icase))) && datestr.empty())
		//if ((std::regex_search(line,tmp2,std::regex("(\\d{1,2})\\/(\\d{1,2})\\/(\\d{2,4})",std::regex_constants::icase)) || std::regex_search(line,tmp2,std::regex("(\\d{1,2})-(\\d{1,2})-(\\d{2,4})",std::regex_constants::icase))) && datestr.empty()) // SFA
		//if (std::regex_search(line,tmp2,std::regex("^(\\d{2})(\\d{2})(\\d{2}) (Noon|[0-9:]+)",std::regex_constants::icase)))
		{
		    int year = std::stoi(tmp2.str(3));
			if (year < 2000)
			{
			    year += 2000;
			}

			int month = std::stoi(tmp2.str(1));

			datestr = std::to_string(year) + "-";
			if (month < 10)
			{
				datestr += "0";
			}
			datestr += std::to_string(month) + "-";

			int day = std::stoi(tmp2.str(2));
			if (day < 10)
			{
				datestr += "0";
			}
			datestr += std::to_string(day);

			// Exhibition check
			//std::cout << datestr << std::endl;
			if (datestr.compare(startdate) < 0)
			{
				datestr = "";
				break;
			}
		}
		else if (std::regex_search(line,tmp2,std::regex("^([A-Za-z0-9&.`\\-()'?_#\\/,\\[\\]; ]+) vs ([A-Za-z0-9&.\\-()`'?_#\\/,\\[\\]; ]+)")) && team1.empty() && team2.empty())
		{
			team1 = tmp2.str(1);
			awaystatline["TEAM"] = std::regex_replace(team1, std::regex("^\\s+"), "");;
			team2 = tmp2.str(2);
			//std::cout << team1 << "," << team2 << std::endl;
			homestatline["TEAM"] = std::regex_replace(team2,std::regex(" \\(\\d.*"), "");
			awayteam.SetTeamName(awaystatline["TEAM"]);
			hometeam.SetTeamName(homestatline["TEAM"]);
		}
		else if (std::regex_search(line,tmp2,std::regex("VISITORS:[ ]+([A-Za-z0-9&.`\\-()'?_#\\/,\\[\\]; ]+)")) && team1.empty())
		{
			team1 = tmp2.str(1);
			awaystatline["TEAM"] = std::regex_replace(team1,std::regex(" \\(\\d.*"), "");
			//std::cout << awaystatline["TEAM"] << std::endl;
			awayteam.SetTeamName(awaystatline["TEAM"]);
		}
		else if (std::regex_search(line,tmp2,std::regex("HOME TEAM:[ ]+([A-Za-z0-9&.`\\-()'?_#\\/,\\[\\]; ]+)")) && team2.empty())
		{
			team2 = tmp2.str(1);
			homestatline["TEAM"] = std::regex_replace(team2,std::regex(" \\(\\d.*"), "");
			//std::cout << homestatline["TEAM"] << std::endl;
			hometeam.SetTeamName(homestatline["TEAM"]);
		}
		else if (std::regex_search(line,tmp2,std::regex(">Totals",std::regex_constants::icase)))
		{
		    totalfound = true;
			++totallabelcount;
		}
		else if ((std::regex_search(line,tmp,std::regex("Totals\\.\\.",std::regex_constants::icase))/* || std::regex_search(line,tmp,std::regex("Totals",std::regex_constants::icase))*/) &&
				 (std::regex_search(line,tmp2,std::regex("(\\d{1,3})-(\\d{1,3})")) || std::regex_search(line,tmp2,std::regex("(\\d{1,3})"))))
		{
		    ++totallabelcount;
			if (totallabelcount < 3)
			{
			    std::istringstream tmpss{line};
				std::vector<std::string> lparts{std::istream_iterator<std::string>(tmpss),
									            std::istream_iterator<std::string>{}};
				for (std::string lp : lparts)
				{
				    //std::cout << "lp = " << lp << std::endl;
				    std::smatch value;
					if (std::regex_search(lp,value,std::regex("(\\d{1,3})-(\\d{1,3})")))
					{
						++numhyphensfound;
						switch (numhyphensfound)
						{
						case 1:
						    //FGM/FGA
							if (totallabelcount == 1)
							{
							    awaystatline["FGM"] = value.str(1);
								awaystatline["FGA"] = value.str(2);
							}
							else if (totallabelcount == 2)
							{
								homestatline["FGM"] = value.str(1);
								homestatline["FGA"] = value.str(2);
							}
							break;

						case 2:
							//3PTM/3PTA
							if (totallabelcount == 1)
							{
							    awaystatline["3PT"] = value.str(1);
							}
							else if (totallabelcount == 2)
							{
								homestatline["3PT"] = value.str(1);
							}
							break;

						case 3:
							//FTM/FTA
							if (totallabelcount == 1)
							{
							    awaystatline["FTM"] = value.str(1);
							    awaystatline["FTA"] = value.str(2);
							}
							else if (totallabelcount == 2)
							{
								homestatline["FTM"] = value.str(1);
								homestatline["FTA"] = value.str(2);
							}
							break;

						case 4:
							//OREB-DREB
							if (totallabelcount == 1)
							{
								awaystatline["OREB"] = value.str(1);
							}
							else if (totallabelcount == 2)
							{
								homestatline["OREB"] = value.str(1);
							}
							break;

						default:
							break;
						}
					}
					else if (std::regex_search(lp,value,std::regex("(\\d{1,3})")))
					{
					    ++numvalsfound;
					    //std::cout << "," << numvalsfound << "," << numhyphensfound << "," << value.str(1) << std::endl;
						switch (numvalsfound)
					    {
						case 1:
							if (numhyphensfound > 0)
							{
							    //OREB
								if (totallabelcount == 1)
								{
								    awaystatline["OREB"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["OREB"] = value.str(1);
								}
							}
							else
							{
							    //FGM
								if (totallabelcount == 1)
								{
								    awaystatline["FGM"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["FGM"] = value.str(1);
								}
							}
							break;

						case 2:
							if (numhyphensfound == 0)
							{
							    //FGM
								if (totallabelcount == 1)
								{
								    awaystatline["FGA"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["FGA"] = value.str(1);
								}
							}
							break;

						case 3:
							if (numhyphensfound > 0)
							{
								// Total rebounds
								if (totallabelcount == 1)
								{
								    awaystatline["REB"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["REB"] = value.str(1);
								}
							}
							else
							{
								// Total rebounds
								if (totallabelcount == 1)
								{
								    awaystatline["3PT"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["3PT"] = value.str(1);
								}
							}
							break;

						case 5:
							if (numhyphensfound > 0)
							{
								// Team points
								if (totallabelcount == 1)
							    {
								    awaystatline["PTS"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["PTS"] = value.str(1);
								}
							}
							else
							{
								// FTM
								if (totallabelcount == 1)
							    {
								    awaystatline["FTM"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["FTM"] = value.str(1);
								}
							}
							break;

						case 6:
							if (numhyphensfound == 0)
							{
								// Team points
								if (totallabelcount == 1)
							    {
								    awaystatline["FTA"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["FTA"] = value.str(1);
								}
							}
							break;

						case 7:
							if (numhyphensfound > 0)
							{
								// turnovers
								if (totallabelcount == 1)
								{
								    awaystatline["TO"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["TO"] = value.str(1);
								}
							}
							else
							{
							    //OREB
								if (totallabelcount == 1)
								{
								    awaystatline["OREB"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["OREB"] = value.str(1);
								}
							}
							break;

						case 8:
							if (numhyphensfound > 0)
							{
								if (totallabelcount == 1)
								{
									awaystatline["MIN"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["MIN"] = value.str(1);
								}
							}
							break;

						case 9:
							if (numhyphensfound == 0)
							{
							    //OREB
								if (totallabelcount == 1)
								{
								    awaystatline["REB"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["REB"] = value.str(1);
								}
							}
							break;
						case 10:
							if (numhyphensfound > 0)
							{
								if (totallabelcount == 1)
								{
									awaystatline["MIN"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["MIN"] = value.str(1);
								}
							}
							break;

						case 11:
							if (numhyphensfound == 0)
							{
								// Team points
								if (totallabelcount == 1)
							    {
								    awaystatline["PTS"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["PTS"] = value.str(1);
								}
							}
							break;

						case 13:
							if (numhyphensfound == 0)
							{
								// turnovers
								if (totallabelcount == 1)
								{
								    awaystatline["TO"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["TO"] = value.str(1);
								}
							}
							break;
						case 16:
							if (numhyphensfound == 0)
							{
								if (totallabelcount == 1)
								{
									awaystatline["MIN"] = value.str(1);
								}
								else if (totallabelcount == 2)
								{
									homestatline["MIN"] = value.str(1);
								}
							}
							break;

						default:
							break;
						}
					}
				}
			}
			numhyphensfound = 0;
			numvalsfound = 0;
		}
		else if (std::regex_search(line,tmp2,std::regex("<\\/tr>")) && totalfound)
		{
		    totalfound = false;
			numhyphensfound = 0;
			numvalsfound = 0;
		}
		else if (std::regex_search(line,tmp2,std::regex(">(\\d{1,3})-(\\d{1,3})")) && totalfound)
		{
			++numhyphensfound;
			switch (numhyphensfound)
			{
			case 1:
			    //FGM/FGA
				if (totallabelcount == 1)
				{
				    awaystatline["FGM"] = tmp2.str(1);
					awaystatline["FGA"] = tmp2.str(2);
				}
				else if (totallabelcount == 2)
				{
					homestatline["FGM"] = tmp2.str(1);
					homestatline["FGA"] = tmp2.str(2);
				}
				break;

			case 2:
				//3PTM/3PTA
				if (totallabelcount == 1)
				{
					awaystatline["3PT"] = tmp2.str(1);
				}
				else if (totallabelcount == 2)
				{
					homestatline["3PT"] = tmp2.str(1);
				}
				break;

			case 3:
				//FTM/FTA
				if (totallabelcount == 1)
				{
				    awaystatline["FTM"] = tmp2.str(1);
					awaystatline["FTA"] = tmp2.str(2);
				}
				else if (totallabelcount == 2)
				{
					homestatline["FTM"] = tmp2.str(1);
					homestatline["FTA"] = tmp2.str(2);
				}
				break;

			case 4:
				//OREB-DREB
				if (totallabelcount == 1)
				{
					awaystatline["OREB"] = tmp2.str(1);
				}
				else if (totallabelcount == 2)
				{
					homestatline["OREB"] = tmp2.str(1);
				}
				break;

			default:
				break;
			}
		}
		else if (std::regex_search(line,tmp2,std::regex(">(\\d{1,3})")) && totalfound)
		{
			++numvalsfound;
			switch (numvalsfound)
		    {
			case 1:
				if (numhyphensfound > 0)
				{
					if (numhyphensfound < 4)
					{
					    //OREB
						if (totallabelcount == 1)
						{
						    awaystatline["OREB"] = tmp2.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["OREB"] = tmp2.str(1);
						}
					}
					else
					{
						// Total rebounds
						if (totallabelcount == 1)
						{
						    awaystatline["REB"] = tmp2.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["REB"] = tmp2.str(1);
						}
					}
				}
				else
				{
				    //FGM
					if (totallabelcount == 1)
					{
					    awaystatline["FGM"] = tmp2.str(1);
					}
					else if (totallabelcount == 2)
					{
						homestatline["FGM"] = tmp2.str(1);
					}
				}
				break;

			case 2:
				if (numhyphensfound == 0)
				{
				    //FGM
					if (totallabelcount == 1)
					{
					    awaystatline["FGA"] = tmp2.str(1);
					}
					else if (totallabelcount == 2)
					{
						homestatline["FGA"] = tmp2.str(1);
					}
				}
				break;

			case 3:
				if (numhyphensfound > 0)
				{
					if (numhyphensfound < 4)
					{
						// Total rebounds
						if (totallabelcount == 1)
						{
						    awaystatline["REB"] = tmp2.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["REB"] = tmp2.str(1);
						}
					}
					else
					{
						// Team points
						if (totallabelcount == 1)
					    {
						    awaystatline["PTS"] = tmp2.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["PTS"] = tmp2.str(1);
						}
					}
				}
				else
				{
					// Total rebounds
					if (totallabelcount == 1)
					{
					    awaystatline["3PT"] = tmp2.str(1);
					}
					else if (totallabelcount == 2)
					{
						homestatline["3PT"] = tmp2.str(1);
					}
				}
				break;

			case 5:
				if (numhyphensfound > 0)
				{
					if (numhyphensfound < 4)
					{
						// Team points
						if (totallabelcount == 1)
					    {
						    awaystatline["PTS"] = tmp2.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["PTS"] = tmp2.str(1);
						}
					}
					else
					{
						// Team points
						if (totallabelcount == 1)
					    {
						    awaystatline["TO"] = tmp2.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["TO"] = tmp2.str(1);
						}
					}
				}
				else
				{
					// FTM
					if (totallabelcount == 1)
				    {
					    awaystatline["FTM"] = tmp2.str(1);
					}
					else if (totallabelcount == 2)
					{
						homestatline["FTM"] = tmp2.str(1);
					}
				}
				break;

			case 6:
				if (numhyphensfound == 0)
				{
					// Team points
					if (totallabelcount == 1)
				    {
					    awaystatline["FTA"] = tmp2.str(1);
					}
					else if (totallabelcount == 2)
					{
						homestatline["FTA"] = tmp2.str(1);
					}
				}
				break;

			case 7:
				if (numhyphensfound > 0)
				{
					if (numhyphensfound < 4)
					{
						// turnovers
						if (totallabelcount == 1)
						{
						    awaystatline["TO"] = tmp2.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["TO"] = tmp2.str(1);
						}
					}
				}
				else
				{
				    //OREB
					if (totallabelcount == 1)
					{
					    awaystatline["OREB"] = tmp2.str(1);
					}
					else if (totallabelcount == 2)
					{
						homestatline["OREB"] = tmp2.str(1);
					}
				}
				break;

			case 8:
				if (numhyphensfound > 0)
				{
					if (numhyphensfound < 4)
					{
					}
					else
					{
						// Team points
						if (totallabelcount == 1)
						{
							awaystatline["MIN"] = tmp2.str(1);
						}
						else if (totallabelcount == 2)
						{
							homestatline["MIN"] = tmp2.str(1);
						}
					}
				}
				break;

			case 9:
				if (numhyphensfound == 0)
				{
				    //OREB
					if (totallabelcount == 1)
					{
					    awaystatline["REB"] = tmp2.str(1);
					}
					else if (totallabelcount == 2)
					{
						homestatline["REB"] = tmp2.str(1);
					}
				}
				break;

			case 11:
				if (numhyphensfound == 0)
				{
					// Team points
					if (totallabelcount == 1)
				    {
					    awaystatline["PTS"] = tmp2.str(1);
					}
					else if (totallabelcount == 2)
					{
						homestatline["PTS"] = tmp2.str(1);
					}
				}
				break;

			case 13:
				if (numhyphensfound == 0)
				{
					// turnovers
					if (totallabelcount == 1)
					{
					    awaystatline["TO"] = tmp2.str(1);
					}
					else if (totallabelcount == 2)
					{
						homestatline["TO"] = tmp2.str(1);
					}
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


