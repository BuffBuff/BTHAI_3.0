#ifndef __BASEAGENT_H__
#define __BASEAGENT_H__

#include <windows.h>
#include <BWAPI.h>
#include <BWTA.h>
#include "BWTAExtern.h"
using namespace BWAPI;
using namespace std;

/** The BaseAgent is the base agent class all agent classes directly or indirectly must extend. It contains some
 * common logic that is needed for several other agent implementations.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class BaseAgent {

private:

protected:
	Unit* unit;
	UnitType type;
	TilePosition goal;
	int unitID;
	int squadID;
	bool alive;
	int lastActionFrame;
	bool bBlock;
	string agentType;

public:
	/** Default constructor. Not used. */
	BaseAgent();
	/** Constructor. */
	BaseAgent(Unit* mUnit);
	/** Destructor. */
	~BaseAgent();

	/** Called each update to issue orders. */
	virtual void computeActions()
	{
	}

	/** Used in debug modes to show a line to the agents' goal. */
	virtual void debug_showGoal()
	{
	}

	/** Returns the number of enemy units within weapon range of the agent. */
	int noUnitsInWeaponRange();

	/** Checks if this unit can attack the specified target unit. */
	bool canAttack(Unit* target);

	/** Checks if this unit can attack the specified unit type. */
	bool canAttack(UnitType type);

	/** Sets the goal for this unit. Goals are set from either the SquadCommander for attacking
	 * or defending units, or from ExplorationManager for explore units. */
	virtual void setGoal(TilePosition goal);

	/** Clears the goal for this unit. */
	virtual void clearGoal();

	/** Returns the current goal for this unit. */
	TilePosition getGoal();

	/** Sets the current frame when this agent is issued an order. */
	void setActionFrame();

	/** Returns the last frame this agent was issued an order. */
	int getLastActionFrame();

	/** Returns the unique type name for the agent type. */
	string getTypeName();

	/** Assigns the agent to build the specified type of unit. */
	virtual bool assignToBuild(UnitType type)
	{
		return false;
	}

	/** Assigns the agent to repair a unit. */
	virtual bool assignToRepair(Unit* building)
	{
		return false;
	}

	/** Assigns the agent to continue building a non-finished building. */
	virtual bool assignToFinishBuild(Unit* building)
	{
		return false;
	}

	/** Assigns this agent to explore the game world. */
	virtual void assignToExplore()
	{
	}

	/** Assigns this agent to be in the attack force. */
	virtual void assignToAttack()
	{
	}

	/** Assigns this agent to defend the base. */
	virtual void assignToDefend()
	{
	}

	/** Returns true if this agent is used to explore the game world. */
	virtual bool isExploring()
	{
		return false;
	}

	/** Returns true if this agent is used to attack the enemy. */
	virtual bool isAttacking()
	{
		return false;
	}

	/** Returns true if this agent is defending the own base. */
	virtual bool isDefending()
	{
		return false;
	}

	/** Used to print info about this agent to the screen. */
	virtual void printInfo();

	/** Returns true if this agent can build units of the specified type. */
	virtual bool canBuild(UnitType type);

	/** Returns the unique id for this agent. Agent id is the same as the id of the unit
	 * assigned to the agent. */
	int getUnitID();

	/** Returns the type for the unit handled by this agent. */
	UnitType getUnitType();

	/** Returns a reference to the unit assigned to this agent. */
	Unit* getUnit();

	/** Called when the unit assigned to this agent is destroyed. */
	virtual void destroyed();

	/** Returns true if this agent is active, i.e. the unit is not destroyed. */
	bool isAlive();

	/** Returns true if the specified unit is the same unit assigned to this agent. */
	bool matches(Unit* mUnit);

	/** Returns true if the agent is of the specified type. */
	bool isOfType(UnitType type);

	/** Returns true if the unit is of the specified type. */
	static bool isOfType(Unit* mUnit, UnitType type);

	/** Returns true if mType is the same UnitType as toCheckType. */
	static bool isOfType(UnitType mType, UnitType toCheckType);

	/** Checks if there are any enemy detector units withing range of the
	 * specified position. True if there is, false if not. */
	bool isDetectorWithinRange(TilePosition pos, int range);

	/** Returns true if this agent is a building. */
	bool isBuilding();

	/** Returns true if this agent is a worker. */
	bool isWorker();

	/** Returns true if this agent is a free worker, i.e. is idle or is gathering minerals. */
	bool isFreeWorker();

	/** Returns true if this agent is a combat unit. */
	bool isUnit();

	/** Returns true if this agent is under attack, i.e. lost hitpoints since last check. */
	bool isUnderAttack();

	/** Returns true if this agent is damaged. */
	bool isDamaged();

	/** Orders the Terran Comsat Station to do a Scanner Sweep at the specified area. */
	static bool doScannerSweep(TilePosition pos);

	/** Orders the Zerg Queen to do an Ensnare at the specified area. */
	static bool doEnsnare(TilePosition pos);

	/** Assigns this agent to the squad with the specified id. */
	void setSquadID(int id);

	/** Returns the squad this agent belongs to, or -1 if it doesnt
	 * belong to any squad. */
	int getSquadID();

};

#endif
