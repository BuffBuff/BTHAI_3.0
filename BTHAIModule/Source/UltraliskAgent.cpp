#include "UltraliskAgent.h"
#include "PFManager.h"
#include "TargetingAgent.h"

UltraliskAgent::UltraliskAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "UltraliskAgent";
	//Broodwar->printf("UltraliskAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void UltraliskAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
