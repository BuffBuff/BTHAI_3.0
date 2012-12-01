#include "GuardianAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "Commander.h"
#include "TargetingAgent.h"

GuardianAgent::GuardianAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "GuardianAgent";
	//Broodwar->printf("GuardianAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void GuardianAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
