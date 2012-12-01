#include "UnitAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "ExplorationManager.h"
#include "SpottedObject.h"

UnitAgent::UnitAgent()
{
	
}

UnitAgent::UnitAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	//Broodwar->printf("UnitAgent created (%s)", unit->getType().getName().c_str());
	dropped = 0;
	agentType = "UnitAgent";

	goal = TilePosition(-1, -1);
}

void UnitAgent::debug_showGoal()
{
	if (!isAlive()) return;
	if (unit->isLoaded()) return;
	if (unit->isBeingConstructed()) return;
	if (!unit->isCompleted()) return;
	
	if (goal.x() >= 0 && unit->isMoving())
	{
		Position a = Position(unit->getPosition());
		Position b = Position(goal);
		Broodwar->drawLine(CoordinateType::Map,a.x(),a.y(),b.x(),b.y(),Colors::Teal);

		Broodwar->drawText(CoordinateType::Map, a.x(), a.y() - 5, "Move (%d,%d)", goal.x(), goal.y());
	}

	if (!unit->isIdle())
	{
		Unit* targ = unit->getOrderTarget();
		if (targ != NULL)
		{
			Position a = Position(unit->getPosition());
			Position b = Position(targ->getPosition());

			if (targ->getPlayer()->isEnemy(Broodwar->self()))
			{
				if (targ->exists())
				{
					Broodwar->drawLine(CoordinateType::Map,a.x(),a.y(),b.x(),b.y(),Colors::Red);
					Broodwar->drawText(CoordinateType::Map, unit->getPosition().x(), unit->getPosition().y(), "Attack %s", targ->getType().getName().c_str());
				}
			}
			else
			{
				if (targ->exists())
				{
					Broodwar->drawLine(CoordinateType::Map,a.x(),a.y(),b.x(),b.y(),Colors::Green);
					Broodwar->drawText(CoordinateType::Map, unit->getPosition().x(), unit->getPosition().y(), "%s", targ->getType().getName().c_str());
				}
			}
		}
	}

	if (unit->isBeingHealed())
	{
		Broodwar->drawCircle(CoordinateType::Map, unit->getPosition().x(), unit->getPosition().y(), 32, Colors::White, false);
	}

	if (unit->getType().isDetector())
	{
		double range = unit->getType().sightRange();
		int x = unit->getPosition().x();
		int y = unit->getPosition().y();
		Broodwar->drawCircle(CoordinateType::Map,x,y,(int)range, Colors::Red, false);
	}
}

void UnitAgent::computeActions()
{
#if DISABLE_UNIT_AI == 0
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, false);
#endif
}

void UnitAgent::computeKitingActions()
{
	int range = (int)(unit->getType().groundWeapon().maxRange() - 5);
	if (unit->getType().isFlyer()) range = (int)(unit->getType().airWeapon().maxRange() - 5);
	
	int eCnt = enemyAttackingUnitsWithinRange(range);
	if (eCnt > 0)
	{

		//If Vulture, drop some mines
		if (isOfType(UnitTypes::Terran_Vulture))
		{
			if (Broodwar->self()->hasResearched(TechTypes::Spider_Mines))
			{
				if (unit->getSpiderMineCount() > 0)
				{
					unit->useTech(TechTypes::Spider_Mines, unit->getPosition());
					return;
				}
			}
		}

		unit->rightClick(Position(Broodwar->self()->getStartLocation()));
		return;
	}
	else
	{
		PFManager::getInstance()->computeAttackingUnitActions(this, goal, false, false);
	}
}

int UnitAgent::enemyUnitsWithinRange(int maxRange)
{
	int eCnt = 0;
	int j = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if ((*i)->exists())
		{
			double dist = unit->getDistance((*i));
			if (dist <= maxRange)
			{
				eCnt++;
			}
		}

		j++;
	}

	return eCnt;
}

int UnitAgent::enemyGroundUnitsWithinRange(int maxRange)
{
	if (maxRange < 0)
	{
		return 0;
	}

	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if (!((*i)->getType().isFlyer()))
		{
			if ((*i)->exists())
			{
				double dist = unit->getDistance((*i));
				if (dist <= maxRange)
				{
					eCnt++;
				}
			}
		}
		
	}

	return eCnt;
}

int UnitAgent::enemySiegedTanksWithinRange(TilePosition center)
{
	int maxRange = 12 * 32 + 16;
	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			if ((*i)->getType().getID() == UnitTypes::Terran_Siege_Tank_Siege_Mode.getID())
			{
				double dist = (*i)->getDistance(Position(center));
				if (dist <= maxRange)
				{
					eCnt++;
				}
			}
		}
	}

	return eCnt;
}

int UnitAgent::enemyGroundAttackingUnitsWithinRange(TilePosition center, int maxRange)
{
	if (maxRange < 0)
	{
		return 0;
	}

	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if ((*i)->exists())
		{
			if (!((*i)->getType().isFlyer() || (*i)->getType().isFlyingBuilding()))
			{
				if (canAttack((*i)->getType(), unit->getType()))
				{
					double dist = (*i)->getDistance(Position(center));
					if (dist <= maxRange)
				{
						eCnt++;
					}
				}
			}
		}
	}

	return eCnt;
}

int UnitAgent::enemyAirUnitsWithinRange(int maxRange)
{
	if (maxRange < 0)
	{
		return 0;
	}

	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if ((*i)->exists())
		{
			if ((*i)->getType().isFlyer() || (*i)->getType().isFlyingBuilding())
			{
				double dist = unit->getDistance((*i));
				if (dist <= maxRange)
				{
					eCnt++;
				}
			}
		}	
	}

	return eCnt;
}

int UnitAgent::enemyAirToGroundUnitsWithinRange(int maxRange)
{
	if (maxRange < 0)
	{
		return 0;
	}

	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if ((*i)->exists())
		{
			UnitType type = (*i)->getType();
			if (type.groundWeapon().targetsGround() || type.airWeapon().targetsGround())
			{
				if ((*i)->getType().isFlyer() || (*i)->getType().isFlyingBuilding())
				{
					double dist = unit->getDistance((*i));
					if (dist <= maxRange)
				{
						eCnt++;
					}
				}
			}
		}	
	}

	return eCnt;
}

int UnitAgent::enemyAirAttackingUnitsWithinRange(TilePosition center, int maxRange)
{
	if (maxRange < 0)
	{
		return 0;
	}

	int eCnt = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		if ((*i)->exists())
		{
			if ((*i)->getType().isFlyer() || (*i)->getType().isFlyingBuilding())
			{
				if (canAttack((*i)->getType(), unit->getType()))
				{
					double dist = (*i)->getDistance(Position(center));
					if (dist <= maxRange)
				{
						eCnt++;
					}
				}
			}
		}
		
	}

	return eCnt;
}

bool UnitAgent::useDefensiveMode()
{
	if (unit->getGroundWeaponCooldown() > 0 || unit->getAirWeaponCooldown() > 0)
	{
		if (enemyAttackingUnitsWithinRange() > 0)
		{
			return true;
		}
	}
	return false;
}

int UnitAgent::enemyAttackingUnitsWithinRange()
{
	return enemyGroundAttackingUnitsWithinRange(unit->getTilePosition(), getGroundRange()) + enemyAirAttackingUnitsWithinRange(unit->getTilePosition(), getAirRange());
}

int UnitAgent::enemyAttackingUnitsWithinRange(int maxRange, TilePosition center)
{
	return enemyGroundAttackingUnitsWithinRange(center, maxRange) + enemyAirAttackingUnitsWithinRange(center, maxRange);
}

int UnitAgent::enemyAttackingUnitsWithinRange(UnitType type)
{
	return enemyGroundAttackingUnitsWithinRange(unit->getTilePosition(), getGroundRange(type)) + enemyAirAttackingUnitsWithinRange(unit->getTilePosition(), getAirRange(type));
}

Unit* UnitAgent::getClosestOrganicEnemy(int maxRange)
{
	Unit* enemy = NULL;
	double bestDist = -1;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			if ((*i)->getType().isOrganic() && !(*i)->getType().isBuilding())
			{
				double cDist = unit->getDistance((*i));
				if (bestDist < 0 || cDist < bestDist)
				{
					bestDist = cDist;
					enemy = (*i);
				}
			}
		}
	}

	return enemy;
}

Unit* UnitAgent::getClosestShieldedEnemy(int maxRange)
{
	Unit* enemy = NULL;
	double bestDist = -1;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			if ((*i)->getShields() > 0)
			{
				double cDist = unit->getDistance((*i));
				if (bestDist < 0 || cDist < bestDist)
				{
					bestDist = cDist;
					enemy = (*i);
				}
			}
		}
	}

	return enemy;
}

Unit* UnitAgent::getClosestEnemyTurret(int maxRange)
{
	Unit* enemy = NULL;
	double bestDist = -1;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			UnitType type = (*i)->getType();
			if (type.isBuilding() && type.canAttack())
			{
				double cDist = unit->getDistance((*i));
				if (bestDist < 0 || cDist < bestDist)
				{
					bestDist = cDist;
					enemy = (*i);
				}
			}
		}
	}

	return enemy;
}

Unit* UnitAgent::getClosestEnemyAirDefense(int maxRange)
{
	Unit* enemy = NULL;
	double bestDist = 100000;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			UnitType type = (*i)->getType();
			
			bool canAttackAir = false;
			if (type.isBuilding())
			{
				if (type.groundWeapon().targetsAir()) canAttackAir = true;
				if (type.airWeapon().targetsAir()) canAttackAir = true;
			}

			if (canAttackAir)
			{
				double cDist = unit->getDistance((*i));
				if (cDist < bestDist)
				{
					bestDist = cDist;
					enemy = (*i);
				}
			}
		}
	}

	if (bestDist >= 0 && bestDist <= maxRange)
	{
		return enemy;
	}
	else
	{
		return NULL;
	}
}

int UnitAgent::friendlyUnitsWithinRange()
{
	return friendlyUnitsWithinRange(192);
}

int UnitAgent::friendlyUnitsWithinRange(int maxRange)
{
	int fCnt = 0;
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isUnit() && !agent->isOfType(UnitTypes::Terran_Medic))
		{
			double dist = unit->getDistance(agent->getUnit());
			if (dist <= maxRange)
			{
				fCnt++;
			}
		}
	}
	return fCnt;
}

int UnitAgent::friendlyUnitsWithinRange(TilePosition tilePos, int maxRange)
{
	int fCnt = 0;
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
		for (int i = 0; i < (int)agents.size(); i++)
		{
		BaseAgent* agent = agents.at(i);
		if (agent->isUnit() && !agent->isOfType(UnitTypes::Terran_Medic))
		{
			double dist = agent->getUnit()->getDistance(Position(tilePos));
			if (dist <= maxRange)
			{
				fCnt++;
			}
		}
	}
	return fCnt;
}

int UnitAgent::getGroundRange()
{
	return getGroundRange(unit->getType());
}

int UnitAgent::getGroundRange(UnitType type)
{
	WeaponType wep1 = type.groundWeapon();
	WeaponType wep2 = type.airWeapon();

	int maxRange = -1;
	if (wep1.targetsGround())
	{
		maxRange = wep1.maxRange();
	}
	if (wep2.targetsGround())
	{
		if (wep2.maxRange() > maxRange)
		{
			maxRange = wep2.maxRange();
		}
	}
	
	return maxRange;
}

int UnitAgent::getAirRange()
{
	return getAirRange(unit->getType());
}

int UnitAgent::getAirRange(UnitType type)
{
	WeaponType wep1 = type.groundWeapon();
	WeaponType wep2 = type.airWeapon();

	int maxRange = -1;
	if (wep1.targetsAir())
	{
		maxRange = wep1.maxRange();
	}
	if (wep2.targetsAir())
	{
		if (wep2.maxRange() > maxRange)
		{
			maxRange = wep2.maxRange();
		}
	}
	
	return maxRange;
}

bool UnitAgent::canAttack(UnitType attacker, UnitType target)
{
	if (!attacker.canAttack())
	{
		return false;
	}

	if (target.isFlyer() || target.isFlyingBuilding())
	{
		if (getAirRange(attacker) >= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (getGroundRange(attacker) >= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void UnitAgent::setGoal(TilePosition goal)
{
	if (unit->getType().isFlyer() || unit->getType().isFlyingBuilding())
	{
		//Flyers, can always move to goals.
		this->goal = goal;
	}
	else
	{
		//Ground units, check if we can reach goal.
		if (ExplorationManager::canReach(this, goal))
		{
			this->goal = goal;
		}
	}
}


void UnitAgent::clearGoal()
{
	goal = TilePosition(-1, -1);
}

void UnitAgent::printInfo()
{
	Broodwar->printf("[%s-%d] SquadID: %d (%d,%d) -> (%d,%d)", agentType.c_str(), unitID, squadID, unit->getTilePosition().x(), unit->getTilePosition().y(), goal.x(), goal.y());
}

bool UnitAgent::chargeShields()
{
	int cShields = unit->getShields();
	int maxShields = unit->getType().maxShields();

	if (cShields < maxShields)
	{
		//Shields are damaged
		BaseAgent* charger = AgentManager::getInstance()->getClosestAgent(unit->getTilePosition(), UnitTypes::Protoss_Shield_Battery);
		if (charger != NULL)
		{
			//Charger has energy
			if (charger->getUnit()->getEnergy() > 0)
			{
				double dist = charger->getUnit()->getTilePosition().getDistance(unit->getTilePosition());
				if (dist <= 15)
				{
					//We have charger nearby. Check if we have enemies around
					int eCnt = enemyAttackingUnitsWithinRange(12 * 32, unit->getTilePosition());
					if (eCnt == 0)
				{
						unit->rightClick(charger->getUnit());
						return true;
					}
				}
			}
		}
	}
	return false;
}
