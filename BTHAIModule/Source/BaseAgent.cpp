#include "BaseAgent.h"
#include "BuildPlanner.h"
#include "AgentManager.h"
#include "ResourceManager.h"
#include "ExplorationManager.h"

BaseAgent::BaseAgent()
{
	alive = true;
	squadID = -1;
	type = UnitTypes::Unknown;
	lastActionFrame = 0;
	goal = TilePosition(-1, -1);
}

BaseAgent::BaseAgent(Unit* mUnit)
{
	unit = mUnit;
	unitID = unit->getID();
	type = unit->getType();
	alive = true;
	squadID = -1;
	lastActionFrame = 0;
	goal = TilePosition(-1, -1);
	agentType = "BaseAgent";
}

BaseAgent::~BaseAgent()
{
	
}

string BaseAgent::getTypeName()
{
	return agentType;
}

void BaseAgent::printInfo()
{
	Broodwar->printf("[%d] (%s)", unitID, getTypeName().c_str());
}

int BaseAgent::getUnitID()
{
	return unitID;
}

UnitType BaseAgent::getUnitType()
{
	return type;
}

Unit* BaseAgent::getUnit()
{
	return unit;
}

bool BaseAgent::matches(Unit *mUnit)
{
	if (isAlive())
	{
		if (mUnit->getID() == unitID)
		{
			return true;
		}
	}
	return false;
}

bool BaseAgent::isOfType(UnitType type)
{
	if (unit->getType().getID() == type.getID())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isOfType(Unit* mUnit, UnitType type)
{
	if (mUnit->getType().getID() == type.getID())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isOfType(UnitType mType, UnitType toCheckType)
{
	if (mType.getID() == toCheckType.getID())
	{
		return true;
	}
	return false;
}

bool BaseAgent::canBuild(UnitType type)
{
	//1. Check if building is being constructed
	if (unit->isBeingConstructed())
	{
		return false;
	}

	//2. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}

	//3. Check canMake
	if (!Broodwar->canMake(unit, type))
	{
		return false;
	}

	//4. All is clear.
	return true;
}

bool BaseAgent::isBuilding()
{
	if (unit->getType().isBuilding())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isWorker()
{
	if (unit->getType().isWorker())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isFreeWorker()
{
	if (unit->getType().isWorker())
	{
		if (unit->isIdle() || unit->isGatheringMinerals())
		{
			if (squadID == -1)
			{
				return true;
			}
		}
	}
	return false;
}

bool BaseAgent::isUnit()
{
	if (unit->getType().isBuilding() || unit->getType().isWorker() || unit->getType().isAddon())
	{
		return false;
	}
	return true;
}

bool BaseAgent::isUnderAttack()
{
	bool attack = false;
	
	if (unit->getShields() < unit->getType().maxShields()) attack = true;
	if (unit->getHitPoints() < unit->getType().maxHitPoints()) attack = true;

	if (attack)
	{
		return unit->isUnderAttack();
	}
	return false;
}

void BaseAgent::destroyed()
{
	alive = false;
}

bool BaseAgent::isAlive()
{
	return alive;
}

bool BaseAgent::isDamaged()
{
	if (unit->getHitPoints() < unit->getType().maxHitPoints())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isDetectorWithinRange(TilePosition pos, int range)
{
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->getType().isDetector())
		{
			double dist = (*i)->getDistance(Position(pos));
			if (dist <= range)
			{
				return true;
			}
		}
	}
	return false;
}

bool BaseAgent::doScannerSweep(TilePosition pos)
{
	if (!BuildPlanner::isTerran())
	{
		return false;
	}

	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive() && agent->isOfType(UnitTypes::Terran_Comsat_Station))
		{
			if (agent->getUnit()->getEnergy() >= 50)
			{
				agent->getUnit()->useTech(TechTypes::Scanner_Sweep, Position(pos));
				return true;
			}
		}
	}

	return false;
}

bool BaseAgent::doEnsnare(TilePosition pos)
{
	if (!BuildPlanner::isZerg())
	{
		return false;
	}
	if (!Broodwar->self()->hasResearched(TechTypes::Ensnare))
	{
		return false;
	}

	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive() && agent->isOfType(UnitTypes::Zerg_Queen))
		{
			if (agent->getUnit()->getEnergy() >= 75)
			{
				agent->getUnit()->useTech(TechTypes::Ensnare, Position(pos));
				return true;
			}
		}
	}

	return false;
}

void BaseAgent::setSquadID(int id)
{
	squadID = id;
}

int BaseAgent::getSquadID()
{
	return squadID;
}

void BaseAgent::setActionFrame()
{
	lastActionFrame = Broodwar->getFrameCount();
}

int BaseAgent::getLastActionFrame()
{
	return lastActionFrame;
}

bool BaseAgent::canAttack(Unit* target)
{
	return canAttack(target->getType());
}

bool BaseAgent::canAttack(UnitType type)
{
	if (!type.isFlyer())
	{
		if (unit->getType().groundWeapon().targetsGround()) return true;
		if (unit->getType().airWeapon().targetsGround()) return true;
	}
	else
	{
		if (unit->getType().groundWeapon().targetsAir()) return true;
		if (unit->getType().airWeapon().targetsAir()) return true;
	}
	return false;
}

int BaseAgent::noUnitsInWeaponRange()
{
	int eCnt = 0;
	for(set<Unit*>::const_iterator i=unit->getUnitsInWeaponRange(unit->getType().groundWeapon()).begin();i!=unit->getUnitsInWeaponRange(unit->getType().groundWeapon()).end();i++)
	{
		if ((*i)->exists() && (*i)->getPlayer()->getID() != Broodwar->self()->getID())
		{
			eCnt++;
		}
	}
	return eCnt;
}

void BaseAgent::setGoal(TilePosition goal)
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

void BaseAgent::clearGoal()
{
	goal = TilePosition(-1, -1);
}

TilePosition BaseAgent::getGoal()
{
	return goal;
}

