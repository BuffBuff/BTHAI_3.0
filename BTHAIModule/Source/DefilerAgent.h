#ifndef __DEFILERAGENT_H__
#define __DEFILERAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The DefilerAgent handles Zerg Defiler units.
 *
 * Implemented special abilities:
 * - Can use Dark Swarm on defensive towers.
 * - Can use Consume on own zerglings to regain energy.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class DefilerAgent : public UnitAgent {

private:
	
public:
	DefilerAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
