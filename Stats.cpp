/*
 * TeamStats.cpp
 *
 *  Created on: Nov 17, 2019
 *      Author: jeremy
 */

#include "Stats.h"
#include <iomanip>

double Stats::CalcPossessions()
{
	return totalfga-totaloffreb+totalto+0.475*totalfta;
}

std::string Stats::GetTeamName()
{
	return teamname;
}

void Stats::SetTeamName(const std::string & nm)
{
	teamname = nm;
}

std::string Stats::GetDateString()
{
	return datestr;
}

void Stats::SetDateString(const std::string & dt)
{
	datestr = dt;
}

double Stats::GetTeamPoints()
{
	return points;
}

void Stats::SetTeamPoints(double pts)
{
	points = pts;
}

double Stats::GetThreePointFieldGoals()
{
	return threepointfgm;
}

void Stats::SetThreePointFieldGoals(double tpfgm)
{
	threepointfgm = tpfgm;
}

double Stats::GetTotalFieldGoals()
{
	return totalfgm;
}

void Stats::SetTotalFieldGoals(double tfgm)
{
	totalfgm = tfgm;
}

double Stats::GetTotalFieldGoalAttempts()
{
	return totalfga;
}

void Stats::SetTotalFieldGoalAttempts(double tfga)
{
	totalfga = tfga;
}

double Stats::GetTotalTurnovers()
{
	return totalto;
}

void Stats::SetTotalTurnovers(double to)
{
	totalto = to;
}

double Stats::GetTotalOffensiveRebounds()
{
	return totaloffreb;
}

void Stats::SetTotalOffensiveRebounds(double tooreb)
{
	totaloffreb = tooreb;
}

double Stats::GetTotalRebounds()
{
	return totalreb;
}

void Stats::SetTotalRebounds(double toreb)
{
	totalreb = toreb;
}

double Stats::GetTotalFreeThrows()
{
	return totalftm;
}

void Stats::SetTotalFreeThrows(double tft)
{
	totalftm = tft;
}

double Stats::GetTotalFreeThrowAttempts()
{
	return totalfta;
}

void Stats::SetTotalFreeThrowAttempts(double tfta)
{
	totalfta = tfta;
}

std::ostream & operator << (std::ostream &out, const Stats &ts)
{
	out << ts.datestr << "," << ts.teamname << ","
		<< ts.threepointfgm << "," << ts.totalfgm << ","
		<< ts.totalfga << "," << ts.totalto << ","
		<< ts.totaloffreb << "," << ts.totalreb << ","
		<< ts.totalftm << "," << ts.totalfta << ","
		<< ts.points;
	return out;
}
