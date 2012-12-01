#ifndef __VALKYRIEAGENT_H__
#define __VALKYRIEAGENT_H__

#include <BWAPI.h>
#include "UnitAgent.h"
using namespace BWAPI;
using namespace std;

/** The ValkyrieAgent handles Terran Valkyrie flying units.
 *
 * Implemented special abilities:
 * - Valkyries doesnt have any special abilities.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ValkyrieAgent : public UnitAgent {

private:

public:
	ValkyrieAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();
};

#endif
