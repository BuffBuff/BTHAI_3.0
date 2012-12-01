#ifndef __REAVERAGENT_H__
#define __REAVERAGENT_H__

#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The ReaverAgent handles Protoss Reaver units.
 *
 * Implemented special abilities:
 * - Produces Scarabs to attack strong enemy groups/units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ReaverAgent : public UnitAgent {

private:
	
public:
	ReaverAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
