#ifndef __UPGRADESPLANNER_H__
#define __UPGRADESPLANNER_H__

#include <BWAPI.h>
#include "BaseAgent.h"

using namespace BWAPI;
using namespace std;

/** UpgradesPlanner contains which updates/techs to be research and in
 * which order.
 *
 * The UpgradesPlanner is implemented as a singleton class. Each class that needs to
 * access UpgradesPlanner can request an instance, and all classes shares the same UpgradesPlanner instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class UpgradesPlanner {

private:
	static UpgradesPlanner* instance;

	UpgradesPlanner();
	
	vector<UpgradeType> upgradesP1;
	vector<UpgradeType> upgradesP2;
	vector<UpgradeType> upgradesP3;
	vector<TechType> techsP1;
	vector<TechType> techsP2;
	vector<TechType> techsP3;

	bool canUpgrade(UpgradeType type, Unit* unit);
	bool canResearch(TechType type, Unit* unit);

public:
	/** Destructor. */
	~UpgradesPlanner();

	/** Returns the instance to the UpgradesPlanner that is currently used. */
	static UpgradesPlanner* getInstance();

	/** Checks if there is an upgrade the specified agent need to upgrade/research. */
	bool checkUpgrade(BaseAgent* agent);
};

#endif
