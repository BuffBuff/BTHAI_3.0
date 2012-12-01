#include "ScienceVesselAgent.h"
#include "PFManager.h"
#include "AgentManager.h"

ScienceVesselAgent::ScienceVesselAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ScienceVesselAgent";
	//Broodwar->printf("ScienceVesselAgent created (%s)", unit->getType().getName().c_str());

	goal = TilePosition(-1, -1);
}

void ScienceVesselAgent::computeActions()
{
	bool defensive = true;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
