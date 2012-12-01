#include "GhostAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "TargetingAgent.h"

GhostAgent::GhostAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "GhostAgent";
	//Broodwar->printf("GhostAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void GhostAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
