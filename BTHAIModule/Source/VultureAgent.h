#ifndef __VULTUREAGENT_H__
#define __VULTUREAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The VultureAgent handles Terran Vulture units.
 *
 * Implemented special abilities:
 * - If there are too many enemies within range, or at least one enemy is within range and the
 * Vulture is damaged, a Spider Mine is dropped and the Vulture retreats to harass the enemy.
 * - Vultures can be used as explorers.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class VultureAgent : public UnitAgent {

private:
	
public:
	VultureAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
