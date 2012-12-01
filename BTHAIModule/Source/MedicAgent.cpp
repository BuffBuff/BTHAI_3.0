#include "MedicAgent.h"
#include "PFManager.h"
#include "AgentManager.h"

MedicAgent::MedicAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "MedicAgent";
	//Broodwar->printf("MedicAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void MedicAgent::computeActions()
{
	bool defensive = true;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}

