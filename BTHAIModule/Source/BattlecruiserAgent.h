#ifndef __BATTLECRUISERAGENT_H__
#define __BATTLECRUISERAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The BattlecruiserAgent handles Terran Battlecruiser flying units.
 *
 * Implemented special abilities:
 * - The Battlecruiser uses Yamato Gun (if researched) to attack enemy turrets or
 * attacking buildings from a distance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class BattlecruiserAgent : public UnitAgent {

private:
	
public:
	BattlecruiserAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
