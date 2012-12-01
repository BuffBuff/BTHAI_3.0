#ifndef __UNITAGENT_H__
#define __UNITAGENT_H__

#include <BWAPI.h>
#include "BaseAgent.h"
using namespace BWAPI;
using namespace std;

#define DISABLE_UNIT_AI 0

/** The UnitAgent is the base agent class for all agents handling mobile units. If a unit is created and no
 * specific agent for that type is found, the unit is assigned to a UnitAgents. UnitAgents can attack and
 * assist building under enemy fire, but cannot use any special abilities. To use abilities such as Terran Vultures
 * dropping spider mines, an agent implementation for that unit type must be created.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class UnitAgent : public BaseAgent {

protected:
	
public:
	UnitAgent(Unit* mUnit);
	UnitAgent();

	int dropped;

	/** Called each update to issue orders. */
	void computeActions();

	/** Used in debug modes to show a line to the agents' goal. */
	void debug_showGoal();

	/** Handles actions for kiting agents. */
	void computeKitingActions();

	/** Sets the goal for this unit. Goals are set from either the SquadCommander for attacking
	 * or defending units, or from ExplorationManager for explore units. */
	virtual void setGoal(TilePosition goal);

	/** Clears the goal for this unit. */
	virtual void clearGoal();

	/** Returns the number of own units that are within 6 tiles range of the agent. */
	int friendlyUnitsWithinRange();

	/** Returns the number of own units that are within maxRange of the agent. */
	int friendlyUnitsWithinRange(int maxRange);

	/** Returns the number of own units that are within maxRange of the specified tile. */
	int friendlyUnitsWithinRange(TilePosition tilePos, int maxRange);

	/** Returns the number of enemy units and buildings that can attack and are within firerange of the agent. */
	int enemyAttackingUnitsWithinRange();

	/** Returns the number of enemy units and buildings that can attack and are within range of the center tile. */
	int enemyAttackingUnitsWithinRange(int maxRange, TilePosition center);

	/** Returns the number of enemy units and buildings that can attack and are within firerange of the specified unit type. */
	int enemyAttackingUnitsWithinRange(UnitType type);

	/** Returns the number of enemy units and buildings that are within maxRange of the agent. */
	int enemyUnitsWithinRange(int maxRange);

	/** Returns the number of enemy ground units and buildings that are within maxRange of the agent. */
	int enemyGroundUnitsWithinRange(int maxRange);

	/** Calculates the number of enemy sieged Siege Tanks within Siege Tank range. */
	int enemySiegedTanksWithinRange(TilePosition center);

	/** Returns the number of enemy ground units and buildings that are within maxRange of the center tile, and can attack the agent. */
	int enemyGroundAttackingUnitsWithinRange(TilePosition center, int maxRange);

	/** Returns the number of flying units that are within maxRange of the agent. */
	int enemyAirUnitsWithinRange(int maxRange);

	/** Checks if a unit can be in defensive mode. This happens if 1) the unit is in cooldown and 2) there are opponent
	 * units that can attack it. */
	bool useDefensiveMode();

	/** Returns the number of flying units that can target ground and are within maxRange of the agent. */
	int enemyAirToGroundUnitsWithinRange(int maxRange);

	/** Returns the number of flying unit that are within maxRange of the center tile, and can attack the agent. */
	int enemyAirAttackingUnitsWithinRange(TilePosition center, int maxRange);

	/** Returns the max firerange of the ground weapon this agent has, or -1 if it cannot attack ground. */
	int getGroundRange();

	/** Returns the max firerange of the ground weapon a unit of the specified type has, or -1 if it cannot attack ground. */
	static int getGroundRange(UnitType type);

	/** Returns true if the attacker UnitType can attack target UnitType. Note: Does not take spells into account, only weapons. */
	static bool canAttack(UnitType attacker, UnitType target);

	/** Returns the max firerange of the air weapon this agent has, or -1 if it cannot attack air. */
	int getAirRange();

	/** Returns the max firerange of the ground weapon a unit of the specified type has, or -1 if it cannot attack air. */
	static int getAirRange(UnitType type);
	
	/** Returns the closest organic enemy unit within maxRange, or NULL if not found. */
	Unit* getClosestOrganicEnemy(int maxRange);

	/** Returns the closest enemy unit that is shielded and within maxRange, or NULL if not found. */
	Unit* getClosestShieldedEnemy(int maxRange);

	/** Returns the closest enemy turret or other attacking building within maxRange, or NULL if not found. */
	Unit* getClosestEnemyTurret(int maxRange);

	/** Returns the closest enemy turret, other attacking building or mechanical unit within maxRange that
	 * can attack air units, or NULL if not found. */
	Unit* getClosestEnemyAirDefense(int maxRange);

	/** Orders a Protoss unit to recharge shields. */
	bool chargeShields();

	/** Used to print info about this agent to the screen. */
	void printInfo();
};

#endif
