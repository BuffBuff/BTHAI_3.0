#include "CorsairAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "TargetingAgent.h"

CorsairAgent::CorsairAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "CorsairAgent";
	//Broodwar->printf("CorsairAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void CorsairAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
