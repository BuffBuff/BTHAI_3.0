#ifndef __AGENTMANAGER_H__
#define __AGENTMANAGER_H__

#include <BWAPI.h>
#include "BaseAgent.h"
#include "cthread.h"

using namespace BWAPI;
using namespace std;

/** The AgentManager class is a container that holds a list of all active agents in the game. Each unit, worker, building or
 * or addon is assigned to an agent. See the MainAgents, StructureAgents and UnitAgents folders for detailed information
 * about each specific type of agent.
 *
 * The AgentManager is implemented as a singleton class. Each class that needs to access AgentManager can request an instance,
 * and all classes shares the same AgentManager instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */

class AgentManager {

private:
	vector<BaseAgent*> agents;
	AgentManager();
	static AgentManager* instance;
	static bool instanceFlag;
	int lastCallFrame;

public:
	static int StartFrame;

	~AgentManager();

	/** Returns the instance to the AgentManager. */
	static AgentManager* getInstance();

	/** Adds an agent to the container. Is called each time a new
	 * unit is built. */
	void addAgent(Unit* unit);

	/** Removes an agent from the container. Is called each time
	 * a unit is destroyed. The agents are not directly removed, but
	 * set to inactive and are removed during the cleanup. */
	void removeAgent(Unit* unit);

	/** Called when a Zerg Drone is morphed into another unit. */
	void morphDrone(Unit* unit);

	/** Called each update to issue commands from all active agents. */
	void computeActions();

	/** Returns the current number of active worker units. */
	int getNoWorkers();

	/** Returns the current number of active workers gathering minerals. */
	int noMiningWorkers();

	/** Returns the closest free worker from the specified position, or NULL if not found. */
	BaseAgent* findClosestFreeWorker(TilePosition pos);

	/** Checks if any agent has the task to repair this specified agent. */
	bool isAnyAgentRepairingThisAgent(BaseAgent* repairedAgent);

	/** Returns the number of own units of a specific type. */
	int countNoUnits(UnitType type);

	/** Returns the number of units or buildings of the specified type
	 * that currently is in production. */
	int noInProduction(UnitType type);

	/** Returns the number of bases the player has. */
	int countNoBases();

	/** Returns a list of all agents in the container. */
	vector<BaseAgent*> getAgents();

	/** Returns the number of agents the exists in the vector. */
	int size();

	/** Returns a reference to the agent associated with a specific unit,
	 * or NULL if the unit doesn't exist. */
	BaseAgent* getAgent(int unitID);

	/** Returns the first agent in the list of the specified type, or NULL if not found. */
	BaseAgent* getAgent(UnitType type);

	/** Requests a free Zerg Overlord to move to the specified position. */
	void requestOverlord(TilePosition pos);

	/** Returns the closest agent in the list of the specified type, or NULL if not found. */
	BaseAgent* getClosestAgent(TilePosition pos, UnitType type);

	/** Returns the closest base agent (Terran Command Center, Protoss Nexus), in the list,
	 * or NULL if not found. */
	BaseAgent* getClosestBase(TilePosition pos);

	/** Returns the position of the closest detector unit relative to startPos. Returns
	 * TilePosition(-1, -1) if none was found. */
	TilePosition getClosestDetector(TilePosition startPos);

	/** Checks if there are any units in an area. The unit with id unitID is allowed. */
	bool unitsInArea(TilePosition pos, int tileWidth, int tileHeight, int unitID);

	/** Removes inactive agents from the container. Shouldn't be called too often. */
	void cleanup();
};

#endif
