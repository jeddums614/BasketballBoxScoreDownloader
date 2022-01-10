/*
 * Utils.cpp
 *
 *  Created on: Nov 16, 2019
 *      Author: jeremy
 */

#include "Utils.h"
#include "DbWrapper.h"
#include "Downloader.h"
#include "IBoxScore.h"
#include "AspxBoxScore.h"
#include "XmlBoxScore.h"
#include "DbmlBoxScore.h"
#include "PdfBoxScore.h"
#include <memory>
#include <regex>
#include <vector>
#include <iostream>
#include <thread>

std::string Utils::exec(const std::string & cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void Utils::Run(int teamId)
{
	std::string teamquery = "select baseurl,scheduleurl,namestosearch,boxscoreformattype,startdate from team where id = " + std::to_string(teamId) + ";";
	std::vector<std::vector<std::string>> teams = DBWrapper::GetResults(teamquery);

	std::vector<std::string> teaminfo = teams[0];

	std::string baseurl = teaminfo[0];
	std::string scheduleurl = teaminfo[1];
	std::string teamname = teaminfo[2];
	BoxScoreFormatType formatType = OTHER;
	if (teaminfo[3].compare("aspx") == 0)
	{
		formatType = ASPX;
	}
	else if (teaminfo[3].compare("xml") == 0)
	{
		formatType = XML;
	}
	else if (teaminfo[3].compare("dbml") == 0)
	{
		formatType = DBML;
	}
	else if (teaminfo[3].compare("pdf") == 0)
	{
		formatType = PDF;
	}
	else
	{
		formatType = OTHER;
	}
	std::string startdate = teaminfo[4];

	std::regex linkregex("href=\"([A-Za-z0-9\\/:.,?=&;_ -]+)\"");
	std::smatch matches;

	std::string schedcontent = Downloader::GetContent(scheduleurl);

	std::vector<std::string> boxscorelinks;
	std::string linkcommand = "google-chrome --headless --dump-dom '"+scheduleurl+"'";
	schedcontent = Utils::exec(linkcommand);

	std::string::const_iterator scit(schedcontent.cbegin());
	while (std::regex_search(scit,schedcontent.cend(),matches,linkregex))
	{
		std::string link = matches.str(1);
		std::smatch tmpmatch;

		//std::cout << link << std::endl; // debug statement
		if ((link.find("boxscore") != std::string::npos &&
			link.find("women") == std::string::npos &&
			(link.find("path=mbb") != std::string::npos ||
			 link.find("mens-basketball") != std::string::npos ||
			 link.find("mbasketball") != std::string::npos ||
			 link.find("/mbball/") != std::string::npos ||
			 link.find("/mbkb/") != std::string::npos ||
			 link.find("path=m_bkb") != std::string::npos ||
			 link.find("m-baskbl") != std::string::npos)) ||
			(formatType == PDF && link.find(".pdf") != std::string::npos))
		{
			if (link.substr(0,4).compare("http") == 0)
			{
				if (std::find(boxscorelinks.begin(),boxscorelinks.end(),link) == boxscorelinks.end())
				{
					if (formatType != PDF) {
						boxscorelinks.push_back(link);
					}
					else {
						if (link.find("/wp-content/") != std::string::npos) {
							boxscorelinks.push_back(link);
						}
						else if (link.find("/documents/") != std::string::npos) {
							std::size_t pos = link.find("//");

							link.insert(pos+2,"s3.amazonaws.com/");
						}
					}
				}
			}
			else
			{
				if (link[0] == '/')
				{
					if (std::find(boxscorelinks.begin(),boxscorelinks.end(),baseurl+link) == boxscorelinks.end())
					{
						if (formatType != PDF) {
							boxscorelinks.push_back(baseurl+link);
						}
						else {
							if (link.find("/wp-content/") != std::string::npos) {
								boxscorelinks.push_back(baseurl+link);
							}
							else if (link.find("/documents/") != std::string::npos) {
								std::size_t pos = baseurl.find("//");

								boxscorelinks.push_back(baseurl.substr(0,pos+2)+"s3.amazonaws.com/"+baseurl.substr(pos+2)+link);
							}
						}
					}
				}
				else
				{
					if (std::find(boxscorelinks.begin(),boxscorelinks.end(),baseurl+"/"+link) == boxscorelinks.end())
					{
						if (formatType != PDF) {
							boxscorelinks.push_back(baseurl + "/" + link);
						}
						else {
							if (link.find("/wp-content/") != std::string::npos) {
								boxscorelinks.push_back(baseurl + "/" + link);
							}
							else if (link.find("/documents/") != std::string::npos) {
						        std::size_t pos = baseurl.find("//");

								boxscorelinks.push_back(baseurl.substr(0,pos+2)+"s3.amazonaws.com/"+baseurl.substr(pos+2)+"/"+link);
							}
						}
					}
				}
			}
		}
		//std::cout << matches.str(1) << std::endl; // debug statement
		scit = matches.suffix().first;
	}

	if (boxscorelinks.size() == 0)
	{
		switch (formatType) {
			case DBML:
			{
				scit = schedcontent.cbegin();
				std::regex bslinkregex("<a href=\"([A-Za-z0-9.\\/&_=?; ]+)\">Box score<\\/a>",std::regex_constants::icase);
				while (std::regex_search(scit,schedcontent.cend(),matches,bslinkregex))
				{
					std::string link = matches.str(1);
					if (link.substr(0,4).compare("http") == 0)
					{
						if (std::find_if(boxscorelinks.begin(), boxscorelinks.end(), [&](const std::string& bslnk) { return bslnk.compare(link) == 0;}) == boxscorelinks.end())
						{
							boxscorelinks.push_back(link);
						}
					}
					else
					{
						if (link[0] == '/')
						{
							if (std::find_if(boxscorelinks.begin(), boxscorelinks.end(), [&](const std::string& bslnk) { return bslnk.compare(baseurl+link) == 0;}) == boxscorelinks.end())
							{
								boxscorelinks.push_back(baseurl+link);
							}
						}
						else
						{
							if (std::find_if(boxscorelinks.begin(), boxscorelinks.end(), [&](const std::string& bslnk) { return bslnk.compare(baseurl+"/"+link) == 0;}) == boxscorelinks.end())
							{
								boxscorelinks.push_back(baseurl+"/"+link);
							}
						}
					}
					scit = matches.suffix().first;
				}
				break;
			}

			case PDF:
			{
				scit = schedcontent.cbegin();
				std::regex bslinkregex("<a href=\"([A-Za-z0-9.\\/:&_=?; -]+)\" target=\"_blank\">*\n *[a-z\"=>]*Box Score \\(PDF\\)",std::regex_constants::icase | std::regex_constants::extended);
				while (std::regex_search(scit,schedcontent.cend(),matches,bslinkregex))
				{
					std::string link = matches.str(1);
					if (link.substr(0,4).compare("http") == 0)
					{
						if (std::find_if(boxscorelinks.begin(), boxscorelinks.end(), [&](const std::string& bslnk) { return bslnk.compare(link) == 0;}) == boxscorelinks.end())
						{
							boxscorelinks.push_back(link);
						}
					}
					else
					{
						if (link[0] == '/')
						{
							if (std::find_if(boxscorelinks.begin(), boxscorelinks.end(), [&](const std::string& bslnk) { return bslnk.compare(baseurl+link) == 0;}) == boxscorelinks.end())
							{
								boxscorelinks.push_back(baseurl+link);
							}
						}
						else
						{
							if (std::find_if(boxscorelinks.begin(), boxscorelinks.end(), [&](const std::string& bslnk) { return bslnk.compare(baseurl+"/"+link) == 0;}) == boxscorelinks.end())
							{
								boxscorelinks.push_back(baseurl+"/"+link);
							}
						}
					}
					scit = matches.suffix().first;
				}
				break;
			}

			default:
				break;
		}
	}

	std::unique_ptr<IBoxScore> boxScoreObj = nullptr;
	switch (formatType)
	{
	case ASPX:
		boxScoreObj = std::make_unique<AspxBoxScore>();
		break;

	case DBML:
		boxScoreObj = std::make_unique<DbmlBoxScore>();
		break;

	case XML:
		boxScoreObj = std::make_unique<XmlBoxScore>();
		break;

	case PDF:
		boxScoreObj = std::make_unique<PdfBoxScore>();
		break;

	default:
		std::cout << "unknown format type" << std::endl;
		break;
	}

	if (boxScoreObj == NULL)
	{
		return;
	}

	int numgames = 0;
	for (std::string bslink : boxscorelinks)
	{
		//std::cout << "boxscore link: " << bslink << std::endl;
		std::optional<std::pair<Stats, Stats>> optstats;
		try
		{
			optstats = boxScoreObj->ProcessUrl(bslink, startdate);
		}
		catch (...)
		{
			std::cout << "unable to process url" << std::endl;
			optstats.reset();
		}

		if (optstats.has_value())
		{
			std::pair<Stats,Stats> gameStats = optstats.value();
			Stats awayteam = std::get<0>(gameStats);
			Stats hometeam = std::get<1>(gameStats);

			++numgames;
			std::cout << std::get<0>(gameStats) << std::endl;
			std::cout << std::get<1>(gameStats) << std::endl;

			if (baseurl.compare("https://miamiredhawks.com") == 0 || baseurl.compare("https://hurricanesports.com") == 0)
			{
				std::string lowername = awayteam.GetTeamName();
				std::for_each(lowername.begin(), lowername.end(), [](char &c) { c = std::tolower(c);});
				std::cout << "lowername = " << lowername << std::endl;

				if (lowername.compare("miami") == 0)
				{
					if (baseurl.compare("https://hurricanesports.com") == 0)
					{
						awayteam.SetTeamName("Miami (FL)");
					}
					else
					{
						awayteam.SetTeamName("Miami (OH)");
					}
				}
				else
				{
					lowername = hometeam.GetTeamName();
					std::for_each(lowername.begin(), lowername.end(), [](char &c) { c = std::tolower(c);});
					std::cout << "lowername2 = " << lowername << std::endl;
					if (lowername.compare("miami") == 0)
					{
						if (baseurl.compare("https://hurricanesports.com") == 0)
						{
							hometeam.SetTeamName("Miami (FL)");
						}
						else
						{
							hometeam.SetTeamName("Miami (OH)");
						}
					}
				}
			}

			std::string awayteamname = awayteam.GetTeamName();
			size_t pos = 0;
			while (std::string::npos != (pos = awayteamname.find("'", pos)))
			{
				awayteamname.replace(pos, 1, "\'\'", 2);
				pos += 2;
			}
			std::string hometeamname = hometeam.GetTeamName();
			pos = 0;
			while (std::string::npos != (pos = hometeamname.find("'", pos)))
			{
				hometeamname.replace(pos, 1, "\'\'", 2);
				pos += 2;
			}
			std::stringstream gamecountquery;
			gamecountquery << "select count(*) from gamebygamedata where "
						   << "(lower(team1)=lower('" << awayteamname << "') "
						   << "or lower(team2)=lower('" << hometeamname << "')) "
						   << "and gamedate='" << awayteam.GetDateString() << "' "
						   << "and team1pts=" << awayteam.GetTeamPoints() << " "
						   << "and team2pts=" << hometeam.GetTeamPoints() << " "
						   << "and team1total3ptfg=" << awayteam.GetThreePointFieldGoals() << " "
						   << "and team1totalfgm=" << awayteam.GetTotalFieldGoals() << " "
						   << "and team1totalfga=" << awayteam.GetTotalFieldGoalAttempts() << " "
						   << "and team1totalto=" << awayteam.GetTotalTurnovers() << " "
						   << "and team1totaloreb=" << awayteam.GetTotalOffensiveRebounds() << " "
						   << "and team1totalreb=" << awayteam.GetTotalRebounds() << " "
						   << "and team1totalftm=" << awayteam.GetTotalFreeThrows() << " "
						   << "and team1totalfta=" << awayteam.GetTotalFreeThrowAttempts() << " "
						   << "and team2total3ptfg=" << hometeam.GetThreePointFieldGoals() << " "
						   << "and team2totalfgm=" << hometeam.GetTotalFieldGoals() << " "
						   << "and team2totalfga=" << hometeam.GetTotalFieldGoalAttempts() << " "
						   << "and team2totalto=" << hometeam.GetTotalTurnovers() << " "
						   << "and team2totaloreb=" << hometeam.GetTotalOffensiveRebounds() << " "
						   << "and team2totalreb=" << hometeam.GetTotalRebounds() << " "
						   << "and team2totalftm=" << hometeam.GetTotalFreeThrows() << " "
						   << "and team2totalfta=" << hometeam.GetTotalFreeThrowAttempts() << ";";
			std::vector<std::vector<std::string>> res = DBWrapper::GetResults(gamecountquery.str());

			int numgames = -1;

			while (res[0][0].empty())
			{
				res = DBWrapper::GetResults(gamecountquery.str());
			}

			std::cout << "value = " << res[0][0] << std::endl;
			try
			{
				numgames = std::stoi(res[0][0]);
			}
			catch (std::invalid_argument &e)
			{
				std::cout << "stoi invalid argument: " << e.what() << std::endl;
				numgames = -1;
			}

			if (numgames == 0)
			{
				std::stringstream query;
				query << "insert into gamebygamedata (gamedate,team1,team1pts,team1total3ptfg,team1totalfgm,team1totalfga,team1totalto,team1totaloreb,team1totalreb,team1totalftm,team1totalfta,team2,team2pts,team2total3ptfg,team2totalfgm,team2totalfga,team2totalto,team2totaloreb,team2totalreb,team2totalftm,team2totalfta) values ('";
				query << awayteam.GetDateString() << "','" << awayteamname
					  << "'," << awayteam.GetTeamPoints() << "," << awayteam.GetThreePointFieldGoals()
					  << "," << awayteam.GetTotalFieldGoals() << "," << awayteam.GetTotalFieldGoalAttempts()
					  << "," << awayteam.GetTotalTurnovers() << "," << awayteam.GetTotalOffensiveRebounds()
					  << "," << awayteam.GetTotalRebounds() << "," << awayteam.GetTotalFreeThrows()
					  << "," << awayteam.GetTotalFreeThrowAttempts() << ",'" << hometeamname
					  << "'," << hometeam.GetTeamPoints() << "," << hometeam.GetThreePointFieldGoals()
					  << "," << hometeam.GetTotalFieldGoals() << "," << hometeam.GetTotalFieldGoalAttempts()
					  << "," << hometeam.GetTotalTurnovers() << "," << hometeam.GetTotalOffensiveRebounds()
					  << "," << hometeam.GetTotalRebounds() << "," << hometeam.GetTotalFreeThrows()
					  << "," << hometeam.GetTotalFreeThrowAttempts() << ");";

				if (DBWrapper::AddEntry(query.str()))
				{
					std::cout << "entry added" << std::endl;
				}
				else
				{
					std::cout << "entry not added" << std::endl;
				}
			}
		}
	}

	boxScoreObj.reset();
}
