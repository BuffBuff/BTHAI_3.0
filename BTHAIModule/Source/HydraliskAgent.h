#ifndef __HYDRALISKAGENT_H__
#define __HYDRALISKAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The HydraliskAgent handles Zerg Hydralisk units.
 *
 * Implemented special abilities:
 * - 
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class HydraliskAgent : public UnitAgent {

private:

public:
	HydraliskAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
