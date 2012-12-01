#ifndef __CORSAIRAGENT_H__
#define __CORSAIRAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The CorsairAgent handles Protoss Corsair flying units.
 *
 * Implemented special abilities:
 * - If researched, uses Disruption Web to take out enemy air defense.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class CorsairAgent : public UnitAgent {

private:
	
public:
	CorsairAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
