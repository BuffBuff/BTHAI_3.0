#include "DefilerAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "TargetingAgent.h"

DefilerAgent::DefilerAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "DefilerAgent";
	//Broodwar->printf("DefilerAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}


void DefilerAgent::Plague(Unit* target)
{
	if(!target->isPlagued())
	{
		unit->useTech(TechTypes::Plague, target->getPosition());
		return;
	}
	else
	{
		DarkSwarm(target);
		return;
	}
}

void DefilerAgent::DarkSwarm(Unit* target)
{
	if(!target->isUnderDarkSwarm())
	{
		unit->useTech(TechTypes::Dark_Swarm, target->getPosition());
		return;
	}
}

void DefilerAgent::Consume(Unit* target)
{
	
}

void DefilerAgent::computeActions()
{
	Unit* target;
	if(enemyUnitsWithinRange(9) > 0)
	{
	Broodwar->printf("I see %d enemy units", enemyUnitsWithinRange(12));
			
	
	target = getClosestEnemy(9);
			if(target == NULL)
				return;

			//Plauge
			if(this->getUnit()->getEnergy() > 150)
			{
				Plague(target);
				return;
			}
			//Dark swarm
			else if(getUnit()->getEnergy() > 100)
			{
				DarkSwarm(target);
				return;
			}
			//Consume
			else
			{
				Consume(target);				
				return;
			}


	bool defensive = true;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
