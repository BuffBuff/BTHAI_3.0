#include "GoliathAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "TargetingAgent.h"

GoliathAgent::GoliathAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "GoliathAgent";
	//Broodwar->printf("GoliathAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void GoliathAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
