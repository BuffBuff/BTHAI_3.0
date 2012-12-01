#ifndef __FIREBATAGENT_H__
#define __FIREBATAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The FirebatAgent handles Terran Firebat units.
 *
 * Implemented special abilities:
 * - The firebat uses stim pack (if researched) when there are enemy units within firerange
 * and the health of the firebat is not too low for the unit to be stimmed.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class FirebatAgent : public UnitAgent {

private:

public:
	FirebatAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
