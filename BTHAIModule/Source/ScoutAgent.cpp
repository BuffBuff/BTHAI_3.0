#include "ScoutAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "TargetingAgent.h"

ScoutAgent::ScoutAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ScoutAgent";
	//Broodwar->printf("ScoutAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void ScoutAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
