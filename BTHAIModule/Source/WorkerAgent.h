#ifndef __WORKERAGENT_H__
#define __WORKERAGENT_H__

#include "BaseAgent.h"
using namespace BWAPI;
using namespace std;

/** The WorkerAgent class handles all tasks that a worker, for example a Terran SCV, can perform. The tasks
 * involves gathering minerals and gas, move to a selected buildspot and construct the specified building,
 * and if Terran SCV, repair a building or tank.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class WorkerAgent : public BaseAgent {

private:
	int currentState;

	UnitType toBuild;
	TilePosition buildSpot;
	TilePosition startSpot;
	bool buildSpotExplored();
	bool areaFree();
	bool isBuilt();
	int startBuildFrame;

	void handleKitingWorker();
	Unit* getEnemyUnit();
	Unit* getEnemyBuilding();
	Unit* getEnemyWorker();

	int lastFrame;

public:
	/** Worker is gathering minerals. */
	static const int GATHER_MINERALS = 0;
	/** Worker is gathering gas. */
	static const int GATHER_GAS = 1;
	/** Worker is trying to find a buildspot for a requested building. */
	static const int FIND_BUILDSPOT = 2;
	/** Worker is moving to a found buildspot. */
	static const int MOVE_TO_SPOT = 3;
	/** Worker is constructing a building. */
	static const int CONSTRUCT = 4;
	/** Worker is repairing a building (Terran only). */
	static const int REPAIRING = 5;
	/** Worker is needed to attack an enemy intruder in a base. */
	static const int ATTACKING = 6;

	/** Constructor. */
	WorkerAgent(Unit* mUnit);

	/** Called each update to issue orders. */
	void computeActions();

	/** Used in debug modes to show a line to the agents' goal. */
	void debug_showGoal();

	/** Set the state of the worker. I.e. what does it do right now. 
	 * Should only be set if the worker is getting a task not through the functions in this class. Then it is automatic. */
	void setState(int state);

	/** Returns the current state of the worker. */
	int getState();

	/** Returns true if the Worker agent can create units of the specified type. */
	bool canBuild(UnitType type);

	/** Assigns the agent to repair a building or tank. */
	bool assignToRepair(Unit* building);

	/** Assigns the unit to construct a building of the specified type. */
	bool assignToBuild(UnitType type);

	/** Assigns the agent to continue building a non-finished building. */
	bool assignToFinishBuild(Unit* building);

	/** Returns the state of the agent as text. Good for printouts. */
	string getStateAsText();

	/** Called when the unit assigned to this agent is destroyed. */
	void destroyed();

	/** Resets a worker to gathering minerals. */
	void reset();

	/** Returns true if this worker is in any of the build states, and is constructing
	 * the specified building. */
	bool isConstructing(UnitType type);
};

#endif
