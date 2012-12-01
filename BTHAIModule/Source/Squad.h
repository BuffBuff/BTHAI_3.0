#ifndef __SQUAD_H__
#define __SQUAD_H__

#include "BaseAgent.h"
#include "Squad.h"
#include "UnitSetup.h"

using namespace BWAPI;
using namespace std;

/** The Squad class represents a squad of units with a shared goal, for example
 * attacking the enemy or defending the base. The Squad can be built up from
 * different combinations and numbers of UnitTypes. 
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class Squad {

protected:
	vector<BaseAgent*> agents;
	vector<UnitSetup> setup;
	UnitType morphs;

	TilePosition goal;
	vector<TilePosition> path;
	int pathIndex;
	int arrivedFrame;
	
	void setMemberGoals(TilePosition cGoal);
	Unit* findTarget();
	
	int id;
	bool active;
	int type;
	int priority;
	int activePriority;
	int moveType;
	bool required;
	string name;
	int goalSetFrame;
	int currentState;

	void checkAttack();
	bool isVisible(TilePosition pos);
	vector<TilePosition> hasVisited;

	/** Sets the goal for this Squad. */
	void setGoal(TilePosition mGoal);

public:
	/** Default constructor. */
	Squad();
	
	/** Creates a squad with a unique id, a type (Offensive, Defensive, Exploration, Support),
	 * a name (for example AirAttackSquad, MainGroundSquad).
	 * Higher priority squads gets filled before lower prio squads. Lower prio value is considered
	 * higher priority. A squad with priority of 1000 or more will not be built. This can be used
	 * to create one-time squads that are only filled once.
	 */
	Squad(int mId, int mType, string mName, int mPriority);

	/** Returns the id for this Squad. */
	int getID();

	/** Adds a requirement that needs to be fulfilled before this Squad is filled with units.
	 * For example (Protoss_Zealot, 10), then the Squad will be filled once 10 Zealots has been built. */
	void addRequirement(UnitType type, int no);

	/** Returns the next starting area to visit, if opponent has not been spotted. */
	TilePosition getNextStartLocation();

	/** Returns the name of this Squad. */
	string getName();

	/** Returns the unit squad members shall morph to (for example Archons or Lurkers)
	 * UnitType::Unknown if no morph is set. */
	UnitType morphsTo();

	/** Sets the unit squad members shall morph to (for example Archons or Lurkers) */
	void setMorphsTo(UnitType type);

	/** Checks if this Squad is required to be active before an attack is launched. */	
	bool isRequired();

	/** Sets if this Squad is required or not. */
	void setRequired(bool mRequired);

	/** Returns the priority for this Squad. Prio 1 is the highest. */
	int getPriority();

	/** Sets the priority for this Squad. Prio 1 is the highest. */
	void setPriority(int mPriority);

	/** Sets the priority this Squad has once it has been active. Prio 1 is the highest. */
	void setActivePriority(int mPriority);

	/** Adds a setup for this Squad. Setup is a type and amount of units
	 * that shall be in this Squad. */
	virtual void addSetup(UnitType type, int no);

	/** Returns all setups for this Squad. Each UnitType in the Squad
	 * is one setup. */
	vector<UnitSetup> getSetup();

	/** Returns true if this Squad is active, or false if not.
	 * A Squad is active when it first has been filled with agents.
	 * A Squad with destroyed units are still considered Active. */
	virtual bool isActive();

	/** Forces an offensive squad to be active, even if it's not full. */
	void forceActive();

	/** Returns true if this Squad is full, i.e. it has all the units
	 * it shall have. */
	bool isFull();

	/** Returns the current size (i.e. number of alive agents in the squad). */
	int size();

	/** Returns the health percentage for this Squad. Health pct means how
	 * full the Squad is. An empty Squad returns 0, a full Squad 100 and a
	 * half full Squad 75. */
	int getHealthPct();

	/** Called each update to issue orders. */
	virtual void computeActions();

	/** Used in debug modes to show goal of squads. */
	void debug_showGoal();

	/** Returns true if this Squad is gathered, i.e. if 90% of its units
	 * are within a certain range of each other. */
	bool isGathered();

	/** Checks if the squad is attacking, i.e. if any member of the squad has targets within range. */
	bool isAttacking();

	/** Returns true if this Squad is under attack. */
	bool isUnderAttack();

	/** Check if this Squad need units of the specified type. */
	bool needUnit(UnitType type);

	/** Adds an agent to this Squad. */
	bool addMember(BaseAgent* agent);

	/** Returns the members of this Squad. */
	vector<BaseAgent*> getMembers();

	/** Removes an agent from this Squad. */
	void removeMember(BaseAgent* agent);

	/** Removes an agent of the specified type from this Squad,
	 * and returns the reference to the removed agent. */
	BaseAgent* removeMember(UnitType type);

	/** Orders this squad to defend a position. */
	virtual void defend(TilePosition mGoal);

	/** Orders this squad to launch an attack at a position. */
	virtual void attack(TilePosition mGoal);

	/** Orders this squad to assist units at a position. */
	virtual void assist(TilePosition mGoal);

	/** Clears the goal for this Squad, i.e. sets the goal
	 * to TilePosition(-1,-1). */
	virtual void clearGoal();

	/** Returns the current goal of this Squad. */
	virtual TilePosition getGoal();

	/** Returns the next TilePosition to move to. */
	TilePosition nextMovePosition();

	/** Returns true if this squad has an assigned goal. */
	virtual bool hasGoal();

	/** Returns true if the goal of this squad is the same as the
	 * specified goal, false if not. */
	bool isThisGoal(TilePosition mGoal);

	/** Returns true if the goal of this squad is close to the specified goal. */
	bool isCloseTo(TilePosition mGoal);

	/** Returns the center position of this Squad, i.e. the
	 * average x and y position of its members. */
	TilePosition getCenter();

	/** Returns the unit closest to the center position of the Squad. */
	BaseAgent* getCenterAgent();

	/** Returns true if this is an Offensive Squad. */
	bool isOffensive();

	/** Returns true if this is a Defensive Squad. */
	bool isDefensive();

	/** Returns true if this is a Support Squad. */
	bool isSupport();

	/** Returns true if this is an Explorer Squad. */
	bool isExplorer();

	/** Returns true if this is a Bunker Defense Squad. */
	bool isBunkerDefend();

	/** Returns true if this is a Special Squad. */
	bool isSpecial();

	/** Returns true if this squad travels by ground. */
	bool isGround();

	/** Returns true if this squad travels by air. */
	bool isAir();

	/** Returns the size of the Squad, i.e. the number
	 * if agents currently in it. */
	int getSize();

	/** Returns the total number of units in the squad when it is full. */
	int getTotalUnits();

	/** Returns the current strength of the Squad, i.e.
	 * the sum of the destroyScore() for all Squad members. */
	int getStrength();

	/** Disbands this Squad and send its remaining members to
	 * a retreat point. */
	void disband(TilePosition retreatPoint);

	/** Used to print some info to the screen. */
	virtual void printInfo();

	/** Used to print some info to the screen. 
	 * Shows printInfo() plus all members of the
	 * Squad. */
	void printFullInfo();

	/** Checks if this Squad can merge with the specified Squad.
	 * if they can, a merge is performed. */
	bool canMerge(Squad* squad);

	/** Returns true if this Squad has the number of the specified
	 * unit types in it. */
	bool hasUnits(UnitType type, int no);

	/** Checks if the squad has at least one unit of the specified type. */
	bool contains(UnitType type);

	/** Returns the closest start location to the specified position. */
	TilePosition getClosestStartLocation(TilePosition pos);

	/** Offensive Squad */
	static const int OFFENSIVE = 0;
	/** Defensive Squad */
	static const int DEFENSIVE = 1;
	/** Explorer Squad */
	static const int EXPLORER = 2;
	/** Support Squad */
	static const int SUPPORT = 3;
	/** Bunker Defense Squad */
	static const int BUNKER = 4;
	/** Special Squad */
	static const int SPECIAL = 5;
	/** Ground Squad */
	static const int GROUND = 0;
	/** Air Squad */
	static const int AIR = 1;

	/** Squad is attacking */
	static const int STATE_ATTACK = 1;
	/** Squad is defending */
	static const int STATE_DEFEND = 2;
	/** Squad is assisting */
	static const int STATE_ASSIST = 3;
	/** No state is set */
	static const int STATE_NOT_SET = 4;

	
};

#endif
