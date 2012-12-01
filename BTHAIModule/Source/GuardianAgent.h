#ifndef __GUARDIANAGENT_H__
#define __GUARDIANAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The GuardianAgent handles Zerg Guardian units.
 *
 * Implemented special abilities:
 * - 
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class GuardianAgent : public UnitAgent {

private:

public:
	GuardianAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
