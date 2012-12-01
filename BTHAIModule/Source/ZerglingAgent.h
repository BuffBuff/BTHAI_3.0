#ifndef __ZERGLINGAGENT_H__
#define __ZERGLINGAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The ZerglingAgent handles Zerg Zergling units.
 *
 * Implemented special abilities:
 * - 
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ZerglingAgent : public UnitAgent {

private:

public:
	ZerglingAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
