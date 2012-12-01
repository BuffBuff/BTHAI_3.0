#include "ObserverAgent.h"
#include "PFManager.h"
#include "AgentManager.h"

ObserverAgent::ObserverAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ObserverAgent";
	//Broodwar->printf("ObserverAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void ObserverAgent::computeActions()
{
	bool defensive = true;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
