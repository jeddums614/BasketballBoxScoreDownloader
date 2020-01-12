/*
 * main.cpp
 *
 *  Created on: Nov 2, 2019
 *      Author: jeremy
 */

#include <iostream>
#include <regex>
#include <vector>
#include "Downloader.h"
#include "IBoxScore.h"
#include "AspxBoxScore.h"
#include "XmlBoxScore.h"
#include "DbmlBoxScore.h"
#include "PdfBoxScore.h"
#include "DbWrapper.h"

enum BoxScoreFormatType {
	ASPX,
	XML,
	DBML,
	PDF,
	OTHER
};

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "<program> <starting index> <end index>" << std::endl;
		std::exit(-33);
	}

	int startid = 0;
	int endid = 0;
	try
	{
		startid = std::stoi(argv[1]);
		endid = std::stoi(argv[2]);
	}
	catch (...)
	{
		std::cout << "unable to convert arguments to int." << std::endl;
		for (int i = 0; i < argc; ++i)
		{
			std::cout << argv[i] << std::endl;
		}
		std::exit(-1234);
	}

	std::string teamquery = "select baseurl,scheduleurl,namestosearch,boxscoreformattype,startdate from team where id >= " + std::to_string(startid) + " and id <= " + std::to_string(endid) + ";";
	std::vector<std::vector<std::string>> teams = DBWrapper::GetResults(teamquery);
	for (std::vector<std::string> teaminfo : teams)
	{
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

		std::regex linkregex("href=\"([A-Za-z0-9\\/:.,?=&;_\\- ]+)\"");
		std::smatch matches;

		std::string schedcontent = Downloader::GetContent(scheduleurl);

		std::vector<std::string> boxscorelinks;
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
						boxscorelinks.push_back(link);
					}
				}
				else
				{
					if (link[0] == '/')
					{
						if (std::find(boxscorelinks.begin(),boxscorelinks.end(),baseurl+link) == boxscorelinks.end())
						{
							boxscorelinks.push_back(baseurl+link);
						}
					}
					else
				    {
						if (std::find(boxscorelinks.begin(),boxscorelinks.end(),baseurl+"/"+link) == boxscorelinks.end())
						{
							boxscorelinks.push_back(baseurl + "/" + link);
						}
					}
				}
			}
			//std::cout << matches.str(1) << std::endl; // debug statement
			scit = matches.suffix().first;
		}

		if (boxscorelinks.size() == 0 && formatType == DBML)
		{
			scit = schedcontent.cbegin();
			std::regex bslinkregex("<a href=\"([A-Za-z0-9.\\/&_=?; ]+)\">Box score<\\/a>",std::regex_constants::icase);
			while (std::regex_search(scit,schedcontent.cend(),matches,bslinkregex))
			{
				std::string link = matches.str(1);
				if (link.substr(0,4).compare("http") == 0)
				{
					if (std::find(boxscorelinks.begin(),boxscorelinks.end(),link) == boxscorelinks.end())
					{
					    boxscorelinks.push_back(link);
					}
				}
				else
				{
					if (link[0] == '/')
					{
						if (std::find(boxscorelinks.begin(),boxscorelinks.end(),baseurl+link) == boxscorelinks.end())
						{
							boxscorelinks.push_back(baseurl+link);
						}
						//boxscorelinks.push_back(baseurl + link);
					}
					else
				    {
						if (std::find(boxscorelinks.begin(),boxscorelinks.end(),baseurl+"/"+link) == boxscorelinks.end())
						{
							boxscorelinks.push_back(baseurl+"/"+link);
						}
						//boxscorelinks.push_back(baseurl + "/" + link);
					}
				}
				scit = matches.suffix().first;
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
			continue;
		}

		int numgames = 0;
		/*double totalpossessions = 0;
		double totalthreepointfg = 0;
		double totalfg = 0;
		double totalfga = 0;
		double totalto = 0;
		double totaloffreb = 0;
		double totalreb = 0;
		double totalftm = 0;
		double totalfta = 0;*/
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
	        		std::string lowername;
	        		std::transform(awayteam.GetTeamName().begin(),awayteam.GetTeamName().end(),lowername.begin(),::tolower);
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
	        			lowername = "";
	        			std::transform(hometeam.GetTeamName().begin(),hometeam.GetTeamName().end(),lowername.begin(),::tolower);
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
	        	std::string gamecountquery = "select count(*) from gamebygamedata where (lower(team1) = lower('"+awayteamname+"') and lower(team2) = lower('"+hometeamname+"')) and gamedate='"+awayteam.GetDateString()+"';";
	        	std::vector<std::vector<std::string>> res = DBWrapper::GetResults(gamecountquery);

	        	int numgames = std::stoi(res[0].at(0));

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
						  << "," << hometeam.GetTotalFreeThrowAttempts() << ")";

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

	return 0;
}
