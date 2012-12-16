#ifndef __UPGRADESFILEREADER_H__
#define __UPGRADESFILEREADER_H__

#include <BWAPI.h>
#include "FileReaderUtils.h"

using namespace BWAPI;
using namespace std;

struct UpgradePlan
{
	UpgradeType type;
	int frameDelay;
};

struct TechPlan
{
	TechType type;
	int frameDelay;
};

/** This class reads the Upgrade/Techs scriptfile.
 * Upgrades/techs are in three priorities, 1 2 and 3. 1 is highest.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class UpgradesFileReader : public FileReaderUtils {

private:
	void addUpgrade(string line);
	
	vector<UpgradePlan> upgradesP1;
	vector<UpgradePlan> upgradesP2;
	vector<UpgradePlan> upgradesP3;
	vector<TechPlan> techsP1;
	vector<TechPlan> techsP2;
	vector<TechPlan> techsP3;

public:
	/** Constructor. */
	UpgradesFileReader();

	/** Reads the upgrades from file.*/
	void readUpgrades();

	/** Returns upgrades prio 1. */
	vector<UpgradePlan> getUpgradesP1();
	/** Returns upgrades prio 2. */
	vector<UpgradePlan> getUpgradesP2();
	/** Returns upgrades prio 3. */
	vector<UpgradePlan> getUpgradesP3();
	/** Returns techs prio 1. */
	vector<TechPlan> getTechsP1();
	/** Returns techs prio 2. */
	vector<TechPlan> getTechsP2();
	/** Returns techs prio 3. */
	vector<TechPlan> getTechsP3();
};

#endif
