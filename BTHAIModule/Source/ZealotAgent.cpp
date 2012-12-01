#include "ZealotAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "Commander.h"
#include "Squad.h"
#include "TargetingAgent.h"
#include "Profiler.h"

ZealotAgent::ZealotAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ZealotAgent";
	//Broodwar->printf("ZealotAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void ZealotAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
