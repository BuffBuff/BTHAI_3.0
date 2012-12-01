#ifndef __PFMANAGER_H__
#define __PFMANAGER_H__

#include <BWAPI.h>
#include "BaseAgent.h"

using namespace BWAPI;
using namespace std;

/** In the bot unit navigation uses two techniques; if no enemy units are close units navigate using the built in pathfinder in
 * Starcraft. If enemy units are close, own units uses potential fields to engage and surround the enemy.
 * The PFManager class is the main class for the potential fields navigation system, and it shall be used compute and execute
 * movement orders using potential fields.
 *
 * The PFManager is implemented as a singleton class. Each class that needs to access PFManager can request an instance,
 * and all classes shares the same PFManager instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class PFManager {

private:
	PFManager();
	static PFManager* instance;
	static bool instanceFlag;

	float getAttackingUnitP(BaseAgent* agent, int cX, int cY, bool defensive);
	
	int checkRange;
	int stepSize;
	int mapW;
	int mapH;

public:
	/** Destructor */
	~PFManager();

	/** Returns the instance to the class. */
	static PFManager* getInstance();

	/** Is used to compute and execute movement commands for attacking units using the potential field
	 * navigation system. */
	void computeAttackingUnitActions(BaseAgent* agent, TilePosition goal, bool defensive);

	/** Is used to compute and execute movement commands for attacking units using the potential field
	 * navigation system. If forceMove is set to true, units always move even if they can attack. */
	void computeAttackingUnitActions(BaseAgent* agent, TilePosition goal, bool defensive, bool forceMove);

	/** Moves a unit to the specified goal using the pathfinder, and stops at a distance where the
	* potential field navigation system should be used instead. */
	bool moveToGoal(BaseAgent* agent, TilePosition checkpoint, TilePosition goal);
	
};

#endif
