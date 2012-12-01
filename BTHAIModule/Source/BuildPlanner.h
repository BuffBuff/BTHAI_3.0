#ifndef __BUILDPLANNER_H__
#define __BUILDPLANNER_H__

#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

struct BuildQueueItem {
	UnitType toBuild;
	int assignedFrame;
	int assignedWorkerId;
};

/** The BuildPlanner class contains the build order for all buildings and addons that will be constructed during the course
 * of a game. This class is not directly used, instead each race (Terran, Protoss, Zerg) has their own build order class
 * that extends BuildPlanner.
 *
 * The BuildPlanner is implemented as a singleton class. Each class that needs to access BuildPlanner can request an instance,
 * and all classes shares the same BuildPlanner instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class BuildPlanner {

private:
	static BuildPlanner* instance;

protected:
	BuildPlanner();
	vector<UnitType> buildOrder;
	vector<BuildQueueItem> buildQueue;
	void lock(int buildOrderIndex, int unitId);
	bool executeOrder(UnitType type);
	bool shallBuildSupplyDepot();
	string format(UnitType type);

	bool hasResourcesLeft();
	int mineralsNearby(TilePosition center);

	int lastCommandCenter;

	int lastCallFrame;

public:
	/** Destructor. */
	~BuildPlanner();

	/** Returns the instance to the BuildPlanner that is currently used. */
	static BuildPlanner* getInstance();

	/** Returns the number of units of the specified type currently being produced. */
	int noInProduction(UnitType type);

	/** Called each update to issue orders. */
	void computeActions();

	/** Notifies that an own unit has been destroyed. */
	void buildingDestroyed(Unit* building);

	/** When a request to construct a new building is issued, no construction are
	 * allowed until the worker has moved to the buildspot and started constructing
	 * the building. This is to avoid that the needed resources are not used up by
	 * other build orders. During this time the BuildPlanner is locked, and new 
	 * construction can only be done when unlock has been called. */
	void unlock(UnitType type);

	/** Removes a building from the buildorder. */
	void remove(UnitType type);

	/** Called when a worker that is constructing a building is destroyed. */
	void handleWorkerDestroyed(UnitType type, int workerID);

	/** Sets that a new command center has been built. */
	void commandCenterBuilt();

	/** Shows some debug info on screen. */
	void printInfo();

	/** Is called when no buildspot has been found for the specified type. Gives each buildplanner
	 * an opportunity to handle it. */
	void handleNoBuildspotFound(UnitType toBuild);

	/** Checks if more supply buildings are needed. */
	bool shallBuildSupply();

	/** Checks if a supply is under construction. */
	bool supplyBeingBuilt();

	/** Returns true if next in buildorder is of the specified type. Returns false if
	 * buildorder is empty. */
	bool nextIsOfType(UnitType type);

	/** Returns true if buildorder contains a unit of the specified type. */
	bool containsType(UnitType type);

	/** Adds a building to the buildorder queue. */
	void addBuilding(UnitType type);

	/** Adds a building first in the buildorder queue. */
	void addBuildingFirst(UnitType type);

	/** Requests to expand the base. */
	void expand(UnitType commandCenterUnit);

	/** Adds a refinery to the buildorder list. */
	void addRefinery();

	/** Checks if the specified TilePosition is covered by a detector buildings sight radius. */
	static bool coveredByDetector(TilePosition pos);

	/** Morphs a Zerg drone to a building. */
	bool executeMorph(UnitType target, UnitType evolved);

	/** Returns true if the player is Terran. */
	static bool isTerran();

	/** Returns true if the player is Protoss. */
	static bool isProtoss();

	/** Returns true if the player is Zerg. */
	static bool isZerg();
};

#endif
