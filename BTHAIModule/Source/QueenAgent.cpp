#include "QueenAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "TargetingAgent.h"

QueenAgent::QueenAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "QueenAgent";
	//Broodwar->printf("QueenAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

bool QueenAgent::BioUnitNotParasite(Unit* target) //Possibly needs UnitAgent*
{
	if(target->isParasited())
	{
		return false;
	}
	Broodwar->printf("I'm going to parasite this honkey cracker..!");
	return true;
}

void QueenAgent::Parasite()
{
//Parasite
	Unit* target;
	if (BioUnitNotParasite(target))
		{
			unit->useTech(TechTypes::Parasite, target);
			Broodwar->sendText("You just got AIDS, muthafudgah!");
			return;
		}
}

void QueenAgent::Broodling()
{
	Unit* target;
	unit->useTech(TechTypes::Spawn_Broodlings, target);
	Broodwar->sendText("You just got AIDS, muthafudgah!");
}

void QueenAgent::computeActions()
{
	if(getUnit()->getEnergy() < 150)
	{
		Broodwar->printf("I are funnnny! %d", getUnit()->getEnergy());
		return;
	}
	else
	{
		Broodwar->printf("Off I go then!");
	}

	Unit* target;
	if(enemyUnitsWithinRange(12) > 0)
	{
	Broodwar->printf("I see %d enemy units", enemyUnitsWithinRange(12));
			target = getClosestOrganicEnemy(12);
			if(target == NULL)
				return;

			if(target->getType().isWorker())
			{
				Broodling();
				return;
			}
			/*else if(getUnit()->getEnergy() >= 75)
			{
				Parasite();
				return;
			}*/

	
	}

	bool defensive = true;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}

