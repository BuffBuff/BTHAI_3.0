#ifndef __DEVOURERAGENT_H__
#define __DEVOURERAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The DevourerAgent handles Zerg DevourerAgent units.
 *
 * Implemented special abilities:
 * -
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class DevourerAgent : public UnitAgent {

private:

public:
	DevourerAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
