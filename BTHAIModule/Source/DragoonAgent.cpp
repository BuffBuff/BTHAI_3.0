#include "DragoonAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "Commander.h"
#include "TargetingAgent.h"
#include "Profiler.h"

DragoonAgent::DragoonAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "DragoonAgent";
	//Broodwar->printf("DragoonAgent created (%s)", unit->getType().getName().c_str());
	goal = TilePosition(-1, -1);
}

void DragoonAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
