#include "MutaliskAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "Squad.h"
#include "Commander.h"
#include "TargetingAgent.h"

MutaliskAgent::MutaliskAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "MutaliskAgent";
	//Broodwar->printf("MutaliskAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void MutaliskAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
