/*
 * GameStats.h
 *
 *  Created on: Nov 17, 2019
 *      Author: jeremy
 */

#ifndef STATS_H_
#define STATS_H_

#include <string>

class Stats
{
public:
	std::string GetTeamName();
	void SetTeamName(const std::string & nm);
	std::string GetDateString();
	void SetDateString(const std::string & dt);
	double GetThreePointFieldGoals();
	void SetThreePointFieldGoals(double tpfgm);
	double GetTotalFieldGoals();
	void SetTotalFieldGoals(double tfgm);
	double GetTotalFieldGoalAttempts();
	void SetTotalFieldGoalAttempts(double tfga);
	double GetTotalTurnovers();
	void SetTotalTurnovers(double to);
	double GetTotalOffensiveRebounds();
	void SetTotalOffensiveRebounds(double tooreb);
	double GetTotalRebounds();
	void SetTotalRebounds(double toreb);
	double GetTotalFreeThrows();
	void SetTotalFreeThrows(double tft);
	double GetTotalFreeThrowAttempts();
	void SetTotalFreeThrowAttempts(double tfta);
	double GetTeamPoints();
	void SetTeamPoints(double pts);
	double CalcPossessions();

	friend std::ostream & operator << (std::ostream &out, const Stats &ts);
private:
	std::string teamname;
	std::string datestr;
	double threepointfgm;
	double totalfgm;
	double totalfga;
	double totalto;
	double totaloffreb;
	double totalreb;
	double totalftm;
	double totalfta;
	double points;
};



#endif /* STATS_H_ */
