#include "QueenAgent.h"
#include "PFManager.h"
#include "AgentManager.h"

QueenAgent::QueenAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "QueenAgent";
	//Broodwar->printf("QueenAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void QueenAgent::computeActions()
{
	bool defensive = true;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}

