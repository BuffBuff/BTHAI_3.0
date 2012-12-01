#ifndef __MEDICAGENT_H__
#define __MEDICAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The MedicAgent handles Terran Medics.
 *
 * Implemented special abilities:
 * - Targets and follows own organic units that are in the attacking force.
 * - Heals nearby damaged organic units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class MedicAgent : public UnitAgent {

private:
	
public:
	MedicAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
