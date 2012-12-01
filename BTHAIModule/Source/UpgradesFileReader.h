#ifndef __UPGRADESFILEREADER_H__
#define __UPGRADESFILEREADER_H__

#include <BWAPI.h>
#include "FileReaderUtils.h"

using namespace BWAPI;
using namespace std;


/** This class reads the Upgrade/Techs scriptfile.
 * Upgrades/techs are in three priorities, 1 2 and 3. 1 is highest.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class UpgradesFileReader : public FileReaderUtils {

private:
	void addUpgrade(string line);
	
	vector<UpgradeType> upgradesP1;
	vector<UpgradeType> upgradesP2;
	vector<UpgradeType> upgradesP3;
	vector<TechType> techsP1;
	vector<TechType> techsP2;
	vector<TechType> techsP3;

public:
	/** Constructor. */
	UpgradesFileReader();

	/** Reads the upgrades from file.*/
	void readUpgrades();

	/** Returns upgrades prio 1. */
	vector<UpgradeType> getUpgradesP1();
	/** Returns upgrades prio 2. */
	vector<UpgradeType> getUpgradesP2();
	/** Returns upgrades prio 3. */
	vector<UpgradeType> getUpgradesP3();
	/** Returns techs prio 1. */
	vector<TechType> getTechsP1();
	/** Returns techs prio 2. */
	vector<TechType> getTechsP2();
	/** Returns techs prio 3. */
	vector<TechType> getTechsP3();
};

#endif
