/*
 * DBWrapper.cpp
 *
 *  Created on: Oct 12, 2019
 *      Author: jeremy
 */

#include "DbWrapper.h"
#include <sqlite3.h>
#include <iostream>

std::vector<std::vector<std::string>> DBWrapper::GetResults(const std::string & query)
{
	std::vector<std::vector<std::string>> results;

	if (query.find("select") != 0)
	{
		return results;
	}

	sqlite3* cbbdb;
	int res = sqlite3_open("cbb.db", &cbbdb);
    if (res) {
        std::cerr << "Error open DB " << sqlite3_errmsg(cbbdb) << std::endl;
        return results;
    }

    sqlite3_stmt *stmt;
    int rc = 0;
    do
    {
    	rc = sqlite3_prepare_v2(cbbdb,query.c_str(),-1,&stmt,NULL);
    }
    while (rc != SQLITE_OK);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
		int num_cols = sqlite3_column_count(stmt);
		std::vector<std::string> vtmp(num_cols);
		for (int i = 0; i < num_cols; i++)
		{
			switch (sqlite3_column_type(stmt, i))
			{
			case SQLITE3_TEXT:
				vtmp[i] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
				break;
			case SQLITE_INTEGER:
			{
				int itmp = sqlite3_column_int(stmt, i);
				vtmp[i] = std::to_string(itmp);
			}
			break;
			case SQLITE_FLOAT:
			{
				double dtmp = sqlite3_column_double(stmt, i);
			    vtmp[i] = std::to_string(dtmp);
			}
			break;
			default:
				break;
			}
		}

		results.push_back(vtmp);
    }

    sqlite3_finalize(stmt);
	sqlite3_close(cbbdb);

	return results;
}

bool DBWrapper::AddEntry(const std::string & query)
{
	if (query.find("insert") != 0)
	{
		return false;
	}

	sqlite3* cbbdb;
	int res = sqlite3_open("cbb.db", &cbbdb);
    if (res) {
        std::cerr << "Error open DB " << sqlite3_errmsg(cbbdb) << std::endl;
        return false;
    }

    do
    {
    	res = sqlite3_exec(cbbdb,query.c_str(),NULL,0,NULL);
    }
    while (res != SQLITE_OK);

	sqlite3_close(cbbdb);
	return true;
}
