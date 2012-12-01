#include "Statistics.h"
#include <stdio.h>
#include <BWTA.h>
#include <shlwapi.h>
#include <time.h>

using namespace BWAPI;
//using namespace UnitTypes;

Statistics::Statistics(void)
{
}

Statistics::~Statistics(void)
{
}

void Statistics::WriteStatisticsFile(bool isWinner)
{
	char szFilePath[1024] = "BTHAI\\SCProjects\\BTHAI_Statistics.csv";
	WCHAR szTime[512];
	time_t ltime;
	struct tm today;
	time( &ltime );
	errno_t err = _localtime64_s( &today, &ltime );
    if (err)
    {
        printf(" _localtime64_s failed due to invalid arguments.");
        exit(1);
    }
	BOOL bNewFile = !PathFileExistsA(szFilePath);
	FILE* f = fopen(szFilePath, "a+");
	if(bNewFile == TRUE)
		fprintf(f, "Time;IsWinner;User race;Enemy race;User minerals;Enemy minerals;User gas;Enemy gas;User kills;Enemy kills;User score;Enemy score;Total score;\n");
	wcsftime(szTime, 512, L"%Y-%m-%d %H:%M", &today);
	fprintf(f, "%ws;", szTime); // Time
	fprintf(f, "%d;", isWinner); // 1 for win 0 for lost
	fprintf(f, "%s;", Broodwar->self()->getRace().getName().c_str()); // User race
	fprintf(f, "%s;", Broodwar->enemy()->getRace().getName().c_str()); // Enemy race
	fprintf(f, "%d;", Broodwar->self()->cumulativeMinerals()); // How much minerals user have mined in total
	fprintf(f, "%d;", Broodwar->enemy()->cumulativeMinerals()); // How much minerals enemy have mined in total
	fprintf(f, "%d;", Broodwar->self()->cumulativeGas()); // How much gas user have mined in total
	fprintf(f, "%d;", Broodwar->enemy()->cumulativeGas()); // How much gas enemy have mined in total

	int nTotalUserKilled = 0;
	int nTotalEnemyKilled = 0;
	int nTotalUserScore = 0;
	int nTotalEnemyScore = 0;
	for(std::set<UnitType>::const_iterator unitType=UnitTypes::allUnitTypes().begin();unitType!=UnitTypes::allUnitTypes().end();unitType++)
	{
		nTotalUserKilled += Broodwar->self()->killedUnitCount(*unitType);
		nTotalEnemyKilled += Broodwar->enemy()->killedUnitCount(*unitType);
		nTotalUserScore += nTotalUserKilled * (*unitType).destroyScore();
		nTotalEnemyScore += nTotalEnemyKilled * (*unitType).destroyScore();
	}
	int nTotalScore = nTotalUserScore - nTotalEnemyScore;

	fprintf(f, "%d;", nTotalUserKilled); // How many enemies did the user kill
	fprintf(f, "%d;", nTotalEnemyKilled); // How many enemies did the enemy kill
	fprintf(f, "%d;", nTotalUserScore); //Total kill score for the user
	fprintf(f, "%d;", nTotalEnemyScore); //Total kill score for the enemy
	fprintf(f, "%d;", nTotalScore); //Total score for the user

	fprintf(f, "\n");
	fclose(f);
}
