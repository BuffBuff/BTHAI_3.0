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

void QueenAgent::computeActions()
{
	Unit* target;
	//TODO: Make it able to target.
	if(enemyUnitsWithinRange(12) > 0 && unit->getEnergy() >= 75)
	{
			target = getClosestOrganicEnemy(12);
			if(target == NULL)
				return;
	Broodwar->printf("I see %d enemy units", enemyUnitsWithinRange(12));
		
	if (BioUnitNotParasite(target))
		{
			unit->useTech(TechTypes::Parasite, target);
			Broodwar->sendText("You just got AIDS, muthafudgah!");
			return;
		}
	}
	

	bool defensive = true;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}

