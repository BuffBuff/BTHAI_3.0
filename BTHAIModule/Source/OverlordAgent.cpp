#include "OverlordAgent.h"
#include "PFManager.h"
#include "Commander.h"
#include "AgentManager.h"

OverlordAgent::OverlordAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "OverlordAgent";
	//Broodwar->printf("OverlordAgent created (%s)", unit->getType().getName().c_str());
	
	lastUpdateFrame = Broodwar->getFrameCount();
	goal = TilePosition(-1, -1);
	updateGoal();
}

void OverlordAgent::updateGoal()
{
	BaseAgent* agent = AgentManager::getInstance()->getClosestBase(unit->getTilePosition());
	if (agent != NULL)
	{
		goal = agent->getUnit()->getTilePosition();
		lastUpdateFrame = Broodwar->getFrameCount();
	}
}

void OverlordAgent::computeActions()
{
	if (squadID == -1)
	{
		if (Broodwar->getFrameCount() - lastUpdateFrame > 100)
		{
			updateGoal();
		}
	}

	PFManager::getInstance()->computeAttackingUnitActions(this, goal, true);
}
