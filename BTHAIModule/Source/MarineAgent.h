#ifndef __MARINEAGENT_H__
#define __MARINEAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The MarineAgent handles Terran Marine units.
 *
 * Implemented special abilities:
 * - The marine uses stim pack (if researched) when there are enemy units within firerange
 * and the health of the marine is not too low for the unit to be stimmed.
 * - If there are any free spots in any bunker, the marine enters the bunker.
 * - Marines can be used as explorers.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class MarineAgent : public UnitAgent {

private:
	bool isNeededInBunker();

public:
	MarineAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
