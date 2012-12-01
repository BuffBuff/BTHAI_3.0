#include "VultureAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "Commander.h"
#include "TargetingAgent.h"

VultureAgent::VultureAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "VultureAgent";
	//Broodwar->printf("VultureAgent created (%s)", unit->getType().getName().c_str());

	goal = TilePosition(-1, -1);
}

void VultureAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
