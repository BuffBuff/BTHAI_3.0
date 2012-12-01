#ifndef __COMMANDER_H__
#define __COMMANDER_H__

#include "Squad.h"
#include "BaseAgent.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

struct SortSquadList {
	bool operator()(Squad*& sq1, Squad*& sq2)
	{
		if (sq1->getPriority() != sq2->getPriority())
		{
			return sq1->getPriority() < sq2->getPriority();
		}
		else
		{
			if (sq1->isRequired() && !sq2->isRequired()) return true;
			else return false;
		}
	}
};

/** The Commander class is the heart of deciding high level strategies like when and where to engage the enemy.
 * It is responsible for deciding when to attack the enemy, where to attack him, and when a retreat 
 * is the best option to do.
 * All units are grouped into Squads, and the Commander directs the different Squads to attack
 * or defend positions. Each Squad are in turn responsible for handle the task it has been assigned.
 *
 * The Commander is implemented as a singleton class. Each class that needs to access Commander can request an instance,
 * and all classes shares the same Commander instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class Commander {

private:
	bool chokePointFortified(TilePosition center);
	bool chokePointGuarded(TilePosition center);
	void sortSquadList();
	bool isOccupied(Region* region);
	bool isEdgeChokepoint(Chokepoint* choke);
	TilePosition findDefensePos(Chokepoint* choke);
	double getChokepointPrio(TilePosition center);

	void checkNoSquadUnits();
	void assignUnit(BaseAgent* agent);

	int lastCallFrame;

protected:
	vector<Squad*> squads;
	int currentID;
	static Commander* instance;

	int currentState;
	static const int DEFEND = 0;
	static const int ATTACK = 1;

	Commander();

	/** Used to find where offensive attackin ground squads are, so
	 * air squads doesnt get ahead of other squads when attacking. */
	TilePosition findOffensiveSquadPosition(TilePosition closeEnemy);

public:
	/** Destructor. */
	~Commander();

	/** Returns the instance of the class. */
	static Commander* getInstance();

	/** Called each update to issue orders. */
	void computeActions();

	/** Returns true if an attack against the enemy has been launched. */
	bool isLaunchingAttack();

	/** Used in debug modes to show goal of squads. */
	virtual void debug_showGoal();

	/** Checks if it is time to engage the enemy. This happens when all Required squads
	 * are active. */
	bool shallEngage();

	void updateGoals();

	/** Called each time a unit is created. The unit is then
	 * placed in a Squad. */
	void unitCreated(BaseAgent* agent);

	/** Called each time a unit is destroyed. The unit is then
	 * removed from its Squad. */
	void unitDestroyed(BaseAgent* agent);

	/* Checks if the specified unittype needs to be built. */
	bool needUnit(UnitType type);

	/** Returns the Squad with the specified id, or NULL if not found. */
	Squad* getSquad(int id);

	/** Returns all Squads. */
	vector<Squad*> getSquads();

	/** Returns the position of the closest enemy building from the start position,
	 * or TilePosition(-1,-1) if not found. */
	TilePosition getClosestEnemyBuilding(TilePosition start);

	/** Tries to find a free squad to assist a building. */
	void assistBuilding(BaseAgent* building);

	/** Tries to find a free squad to assist a worker that is under attack. */
	void assistWorker(BaseAgent* worker);

	/** Called when own units are attacked by a cloaked enemy unit. */
	void handleCloakedEnemy(TilePosition pos, Squad* squad);

	/** Checks if there are any removable obstacles nearby, i.e. minerals with less than 20 resources
	 * left. */
	void checkRemovableObstacles();

	/** Forces an attack, even if some squads are not full. */
	void forceAttack();

	/** Shows some info on the screen. */
	void printInfo();

	/** Searches for a chokepoint that is unfortified, i.e. does not contain for example a Bunker or defensive
	 * turret. Returns TilePosition(-1, -1) if no position was found. */
	TilePosition findUnfortifiedChokePoint();
	
	/** Searches for and returns a good chokepoint position to defend the territory. */
	TilePosition findChokePoint();

	/** Checks if a position is covered by psi (Protoss only). */
	bool isPowered(TilePosition pos);

	/** Checks is a position is buildable. */
	bool isBuildable(TilePosition pos);

	/** Checks if there are any unfinished buildings that does not have an SCV working on them. Terran only. */
	bool checkUnfinishedBuildings();

	/** Check if there are any important buildings or units to repair. Terran only. */
	bool checkRepairUnits();

	/** Returns true if the unit is important to assist, false if not. All buildings and large expensive units
	 * such as siege tanks and battlecruisers are considered important, while small units such as marines and
	 * vultures are not considered important. Terran only.*/
	bool isImportantUnit(Unit* unit);

	/** Assigns a worker to repair the specified agent. Terran only.*/
	void repair(BaseAgent* agent);

	/** Assigns a worker to finish constructing an interrupted building. Terran only. */
	void finishBuild(BaseAgent* agent);

	/** The total killScore points of all own, destroyed units (not buildings). */
	int ownDeadScore;

	/** The total killScore points of all enemy, destroyed units (not buildings). */
	int enemyDeadScore;

	/** Adds a bunker squad when a Terran Bunker has been created. */
	void addBunkerSquad();
};

#endif
