#include "MySpecialSquad.h"
#include "UnitAgent.h"
#include "AgentManager.h"
#include "ExplorationManager.h"
#include "Commander.h"

MySpecialSquad::MySpecialSquad(int mId, string mName, int mPriority)
{
	this->id = mId;
	this->type = SPECIAL;
	this->moveType = AIR;
	this->name = mName;
	this->priority = mPriority;
	activePriority = priority;
	active = false;
	required = false;
	goal = Broodwar->self()->getStartLocation();
	goalSetFrame = 0;
	currentState = STATE_NOT_SET;
}

bool MySpecialSquad::isActive()
{
	return active;
}

void MySpecialSquad::defend(TilePosition mGoal)
{
	setGoal(mGoal);
}

void MySpecialSquad::attack(TilePosition mGoal)
{
	setGoal(mGoal);
}

void MySpecialSquad::assist(TilePosition mGoal)
{
	setGoal(mGoal);
}

void MySpecialSquad::computeActions()
{
	//Call this if goal is changed
	//setMemberGoals(goal);
}

void MySpecialSquad::printInfo()
{
	string f = "NotFull";
	if (isFull())
	{
		f = "Full";
	}
	string a = "Inactive";
	if (isActive())
	{
		a = "Active";
	}

	Broodwar->printf("[MySpecialSquad %d] (%s, %s) Goal: (%d,%d) prio: %d", id, f.c_str(), a.c_str(), goal.x(), goal.y(), priority);
}

void MySpecialSquad::clearGoal()
{
	goal = TilePosition(-1, -1);
}

TilePosition MySpecialSquad::getGoal()
{
	return goal;
}

bool MySpecialSquad::hasGoal()
{
	if (goal.x() < 0 || goal.y() < 0)
	{
		return false;
	}
	return true;
}
