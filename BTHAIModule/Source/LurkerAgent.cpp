#include "LurkerAgent.h"
#include "PFManager.h"
#include "Commander.h"
#include "ExplorationManager.h"
#include "TargetingAgent.h"

LurkerAgent::LurkerAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "LurkerAgent";
	//Broodwar->printf("LurkerAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void LurkerAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
