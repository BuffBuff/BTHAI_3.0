#ifndef __FLEETBEACONAGENT_H__
#define __FLEETBEACONAGENT_H__

#include <BWAPI.h>
#include "StructureAgent.h"
using namespace BWAPI;
using namespace std;

/** The FleetBeaconAgent handles Protoss Fleet Beacon buildings.
 *
 * Implemented abilities:
 * - Research Disruption Web
 *
 * TODO:
 * - Add other research/upgrades.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class FleetBeaconAgent : public StructureAgent {

private:

public:
	FleetBeaconAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();

	/** Returns the unique type name for structure agents. */
	string getTypeName();
	
};

#endif
