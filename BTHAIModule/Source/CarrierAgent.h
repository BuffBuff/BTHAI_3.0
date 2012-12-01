#ifndef __CARRIERAGENT_H__
#define __CARRIERAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The CarrierAgent handles Protoss Carrier flying units.
 *
 * Implemented special abilities:
 * - Produces Interceptors.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class CarrierAgent : public UnitAgent {

private:

public:
	CarrierAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
