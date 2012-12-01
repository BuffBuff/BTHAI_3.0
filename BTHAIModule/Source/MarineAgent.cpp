#include "MarineAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "Commander.h"
#include "Squad.h"
#include "TargetingAgent.h"

MarineAgent::MarineAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "MarineAgent";
	//Broodwar->printf("MarineAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

bool MarineAgent::isNeededInBunker()
{
	if (!unit->isLoaded())
	{
		Squad* sq = Commander::getInstance()->getSquad(squadID);
		if (sq != NULL)
		{
			if (sq->isBunkerDefend())
			{
				vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
				for (int i = 0; i < (int)agents.size(); i++)
				{
					if (agents.at(i)->isAlive() && agents.at(i)->isOfType(UnitTypes::Terran_Bunker))
				{
						if (agents.at(i)->getUnit()->getLoadedUnits().size() < 4)
				{
							unit->rightClick(agents.at(i)->getUnit());
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

void MarineAgent::computeActions()
{
	if (isNeededInBunker()) return;

	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
