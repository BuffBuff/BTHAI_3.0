#ifndef __OVERLORDAGENT_H__
#define __ZEALOTAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The OverlordAgent handles Zerg Overlord units.
 *
 * Implemented special abilities:
 * - 
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class OverlordAgent : public UnitAgent {

private:
	int lastUpdateFrame;
	void updateGoal();

public:
	OverlordAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
