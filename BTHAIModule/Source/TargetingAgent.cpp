#include "TargetingAgent.h"
#include "AgentManager.h"

Unit* TargetingAgent::findTarget(BaseAgent* agent)
{
	//Check if the agent targets ground and/or air
	bool targetsGround = false;
	if (agent->getUnitType().groundWeapon().targetsGround()) targetsGround = true;
	if (agent->getUnitType().airWeapon().targetsGround()) targetsGround = true;

	bool targetsAir = false;
	if (agent->getUnitType().groundWeapon().targetsAir()) targetsAir = true;
	if (agent->getUnitType().airWeapon().targetsAir()) targetsAir = true;

	//Check max range
	int range = agent->getUnitType().seekRange();
	if (agent->getUnit()->isSieged()) range = agent->getUnit()->getType().groundWeapon().maxRange();

	//Iterate through enemies to select a target
	int bestTargetScore = -1;
	Unit* target = NULL;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		UnitType t = (*i)->getType();

		bool canAttack = false;
		if (!t.isFlyer() && targetsGround) canAttack = true;
		if ((t.isFlyer() || (*i)->isLifted()) && targetsAir) canAttack = true;

		if (canAttack)
		{
			int d = (int)agent->getUnit()->getDistance((*i));
			if (d <= range)
			{
				int cScore = (int)((double)t.destroyScore() * getTargetModifier(agent->getUnit()->getType(), t));
				
				if (cScore > bestTargetScore)
				{
					bestTargetScore = cScore;
					target = (*i);
				}
			}
		}
	}

	return target;
}

double TargetingAgent::getTargetModifier(UnitType attacker, UnitType target)
{
	//Non-attacking buildings
	if (target.isBuilding() && !target.canAttack()) 
	{
		return 0.1;
	}

	//Terran Goliath prefer air targets
	if (attacker.getID() == UnitTypes::Terran_Goliath.getID())
	{
		if (target.isFlyer()) return 2;
	}

	//Siege Tanks prefer to take out enemy defense buildings
	if (attacker.getID() == UnitTypes::Terran_Siege_Tank_Siege_Mode.getID())
	{
		if (target.isBuilding() && target.canAttack()) return 1.5;
		if (target.getID() == UnitTypes::Terran_Bunker.getID()) return 1.5;
	}

	//Siege Tanks are nasty and have high prio to be killed.
	if (target.getID() == UnitTypes::Terran_Siege_Tank_Siege_Mode.getID())
	{
		return 1.5;
	}

	//Prio to take out detectors when having cloaking units
	if (isCloakingUnit(attacker) && target.isDetector())
	{
		return 2;
	}

	return 1; //Default: No modifier
}

bool TargetingAgent::isCloakingUnit(UnitType type)
{
	if (type.isCloakable()) return true;
	return false;
}
