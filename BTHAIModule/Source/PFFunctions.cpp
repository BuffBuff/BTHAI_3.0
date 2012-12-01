#include "PFFunctions.h"

float PFFunctions::getDistance(Position p1, Position p2)
{
	return (float)p1.getDistance(p2);
}

float PFFunctions::getDistance(Position pos, Unit* unit)
{
	return (float)unit->getDistance(pos);
}

int PFFunctions::getSize(UnitType type)
{
	if (type.getID() == UnitTypes::Zerg_Zergling)
	{
		//We want Zerglings to spread out
		return 16;
	}
	if (type.isWorker())
	{
		return 6;
	}
	if (type.size() == UnitSizeTypes::Small)
	{
		return 6;
	}
	if (type.size() == UnitSizeTypes::Medium)
	{
		return 12;
	}
	if (type.size() == UnitSizeTypes::Large)
	{
		return 16;
	}
	return 12;
}

float PFFunctions::calcOwnUnitP(float d, Unit* unit, Unit* otherOwnUnit)
{
	if (unit->getID() == otherOwnUnit->getID())
	{
		//Dont count collision with yourself...
		return 0;
	}

	float p = 0;

	if (BaseAgent::isOfType(otherOwnUnit, UnitTypes::Terran_Vulture_Spider_Mine))
	{
		//Make sure to avoid mines
		if (d <= 125 + getSize(unit->getType()))
		{
			p = -20;
		}
	}

	if (unit->isCloaked() && !otherOwnUnit->isCloaked())
	{
		//Let cloaked units stay away from non-cloaked units to avoid
		//getting killed by splash damage.
		if (d <= 50 + getSize(unit->getType()))
		{
			p = -20;
		}
	}
	if (unit->getType().getID() == UnitTypes::Zerg_Lurker && otherOwnUnit->getType().getID() == UnitTypes::Zerg_Lurker)
	{
		//Let cloaked units stay away from non-cloaked units to avoid
		//getting killed by splash damage.
		if (d <= 64 + getSize(unit->getType()))
		{
			p = -20;
		}
	}

	if (otherOwnUnit->isIrradiated())
	{
		//Other unit under Irradite. Keep distance.
		if (d <= 64)
		{
			p = -20;
		}
	}
	if (otherOwnUnit->isUnderStorm())
	{
		//Other unit under Psionic Storm. Keep distance.
		if (d <= 64)
		{
			p = -20;
		}
	}

    if (d <= getSize(unit->getType()))
{
        p += -20;
    }

    return p;
}

float PFFunctions::calcAvoidWorkerP(float d, BaseAgent* unit, BaseAgent* oUnit)
{
	if (unit->getUnitID() == oUnit->getUnitID())
	{
		//Dont count collision with yourself...
		return 0;
	}

	if (oUnit->getUnit()->isGatheringMinerals() || oUnit->getUnit()->isGatheringGas())
	{
		//Workers are transparent when they gather resources
		return 0;
	}

	float p = 0;

    if (d <= 20)
{
        p = -20;
    }
	else if (d > 20 && d <= 32)
	{
		p = 1;
	}
	
    return p;
}

float PFFunctions::calcMineP(float d, Unit* unit)
{
	float p = 0;

	if (unit->getType().isFlyer())
	{
		p = 0;
	}
	else if (unit->getType().isFlyingBuilding())
	{
		p = 0;
	}
	else
	{
		if (d <= getSize(unit->getType()))
		{
			p = -40;
		}
	}

	return p;
}

float PFFunctions::calcNavigationP(float d)
{
    float p = (float)200 - (float)0.1 * d;
    if (p < 0)
    {
        p = 0;
    }
    return p;
}

float PFFunctions::getNavigationP(Position cPos, Position goal)
{
    float dist = getDistance(cPos, goal);
    return calcNavigationP(dist);
}

float PFFunctions::calcAttackingUnitP(float d, Unit* attacker, Unit* enemy, bool defensive)
{
	//Check if enemy unit exists and is visible.
	if (!enemy->exists())
	{
		return 0;
	}
	if (!enemy->isVisible())
	{
		return 0;
	}
	if (enemy->isCloaked()) 
	{
		return 0;
	}
	//Check for flying buildings
	if (enemy->getType().isFlyingBuilding() && enemy->isLifted())
	{
		return 0;
	}
	//Check if we can attack the type
	if (enemy->getType().isFlyer() && !attacker->getType().airWeapon().targetsAir())
	{
		return 0;
	}
	if (!enemy->getType().isFlyer() && !attacker->getType().groundWeapon().targetsGround())
	{
		return 0;
	}

	//Calc max wep range
    int myMSD = attacker->getType().groundWeapon().maxRange();
	if (enemy->getType().isFlyer())
	{
		myMSD = attacker->getType().airWeapon().maxRange();
	}
	if (!attacker->getType().canAttack())
	{
		//Unit cannot attack, use sightrange instead
		myMSD = attacker->getType().sightRange();
	}
	if (attacker->getType().getID() == UnitTypes::Terran_Medic.getID())
	{
		myMSD = 6*32;
	}
	if (attacker->getType().getID() == UnitTypes::Protoss_High_Templar.getID())
	{
		myMSD = 6*32;
	}

	//Calc attacker wep range
	int enemyMSD = enemy->getType().groundWeapon().maxRange();
	if (attacker->getType().isFlyer())
	{
		enemyMSD = enemy->getType().airWeapon().maxRange();
	}
    
    float p = 0;

	//Cloaked unit: Watch out for detectors.
	if (attacker->isCloaked() && enemy->getType().isDetector())
	{
		//defensive = true;
		//enemyMSD = (int)(enemy->getType().sightRange() * 1.2);
	}

	
	if (!defensive)
	{
		//Offensive mode -> attack
		if (canAttack(attacker, enemy))
		{
			if (d < myMSD - 5)
			{
				float fact = (float)200 / myMSD;
				p = d * fact;
				if (p < 0)
				{
					p = 0;
				}
			}
			else if (d >= myMSD - 5 && d < myMSD)
			{
				p = (float)200;
			}
			else
			{
				float p = (float)180 - (float)0.1 * d;
				if (p < 0)
				{
					p = 0;
				}
			}
		}
	}
	else
	{
		//Defensive mode -> retreat
		p = (float)(d * 2);
		if (d > enemyMSD * 1.5)
		{
			p = 0;
		}
	}

    return p;
}

bool PFFunctions::canAttack(Unit* ownUnit, Unit* target)
{
	UnitType oType = ownUnit->getType();
	UnitType tType = target->getType();

	if (tType.isFlyer())
	{
		//Own unit is air
		if (oType.groundWeapon().targetsAir())
		{
			return true;
		}
		if (oType.airWeapon().targetsAir())
		{
			return true;
		}
	}
	else
	{
		//Own unit is ground
		if (oType.groundWeapon().targetsGround())
		{
			return true;
		}
		if (oType.airWeapon().targetsGround())
		{
			return true;
		}
	}

	return false;
}

float PFFunctions::calcDefensiveUnitP(float d, Unit* ownUnit, Unit* enemy)
{
	int range = 0;
	if (enemy->getType().canAttack())
	{
		range = (int)(enemy->getType().groundWeapon().maxRange() * 1.5);
	}
	if (!canAttack(enemy, ownUnit))
	{
		range = 0;
	}
    
    float p = (float)(d * 2);
	if (d > range)
	{
		p = 0;
	}

    return p;
}

