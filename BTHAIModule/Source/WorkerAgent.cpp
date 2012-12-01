#include "UnitAgent.h"
#include "WorkerAgent.h"
#include "AgentManager.h"
#include "ExplorationManager.h"
#include "PFManager.h"
#include "CoverMap.h"
#include "BuildPlanner.h"
#include "Commander.h"
#include "ResourceManager.h"
#include "PathFinder.h"
#include "Profiler.h"
#include <sstream>

WorkerAgent::WorkerAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	//Broodwar->printf("WorkerAgent created (%s)", unit->getType().getName().c_str());
	setState(GATHER_MINERALS);
	startBuildFrame = 0;
	startSpot = TilePosition(-1, -1);
	agentType = "WorkerAgent";
}

void WorkerAgent::destroyed()
{
	if (currentState == MOVE_TO_SPOT || currentState == CONSTRUCT || currentState == FIND_BUILDSPOT)
	{
		if (!BuildPlanner::isZerg())
		{
			//Broodwar->printf("Worker building %s destroyed", toBuild.getName().c_str());
			BuildPlanner::getInstance()->handleWorkerDestroyed(toBuild, unitID);
			CoverMap::getInstance()->clearTemp(toBuild, buildSpot);
			setState(GATHER_MINERALS);
		}
	}
}

Unit* WorkerAgent::getEnemyUnit()
{
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			if (!(*i)->getType().isBuilding())
			{
				double dist = unit->getTilePosition().getDistance((*i)->getTilePosition());
				if (dist <= 3)
				{
					return (*i);
				}
			}
		}	
	}
	return NULL;
}

Unit* WorkerAgent::getEnemyBuilding()
{
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			if ((*i)->getType().isBuilding())
			{
				double dist = unit->getTilePosition().getDistance((*i)->getTilePosition());
				if (dist <= 5)
				{
					return (*i);
				}
			}
		}	
	}
	return NULL;
}

Unit* WorkerAgent::getEnemyWorker()
{
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			if ((*i)->getType().isWorker())
			{
				double dist = unit->getTilePosition().getDistance((*i)->getTilePosition());
				if (dist <= 5)
				{
					return (*i);
				}
			}
		}	
	}
	return NULL;
}

void WorkerAgent::handleKitingWorker()
{
	//Kite them around
	/*Squad* sq = Commander::getInstance()->getSquad(squadID);
	if (sq != NULL)
	{
		TilePosition nGoal = ExplorationManager::getInstance()->getNextToExplore(sq);
		if (nGoal.x() >= 0)
		{
			unit->rightClick(Position(nGoal));
			return;
		}
	}*/
	
	//Bring them back to base
	unit->rightClick(Position(Broodwar->self()->getStartLocation()));
}

void WorkerAgent::debug_showGoal()
{
	if (!isAlive()) return;
	if (!unit->isCompleted()) return;
	
	if (currentState == GATHER_MINERALS || currentState == GATHER_GAS)
	{
		Unit* target = unit->getTarget();
		if (target != NULL)
		{
			Position a = Position(unit->getPosition());
			Position b = Position(target->getPosition());
			Broodwar->drawLine(CoordinateType::Map,a.x(),a.y(),b.x(),b.y(),Colors::Teal);
		}
	}

	if (currentState == MOVE_TO_SPOT || currentState == CONSTRUCT)
	{
		if (buildSpot.x() > 0)
		{
			int w = toBuild.tileWidth() * 32;
			int h = toBuild.tileHeight() * 32;

			Position a = Position(unit->getPosition());
			Position b = Position(buildSpot.x()*32 + w/2, buildSpot.y()*32 + h/2);
			Broodwar->drawLine(CoordinateType::Map,a.x(),a.y(),b.x(),b.y(),Colors::Teal);

			Broodwar->drawBox(CoordinateType::Map,buildSpot.x()*32,buildSpot.y()*32,buildSpot.x()*32+w,buildSpot.y()*32+h,Colors::Blue,false);
		}
	}

	if (unit->isRepairing())
	{
		Unit* targ = unit->getOrderTarget();
		if (targ != NULL)
		{
			Position a = Position(unit->getPosition());
			Position b = Position(targ->getPosition());
			Broodwar->drawLine(CoordinateType::Map,a.x(),a.y(),b.x(),b.y(),Colors::Green);

			Broodwar->drawText(CoordinateType::Map, unit->getPosition().x(), unit->getPosition().y(), "Repairing %s", targ->getType().getName().c_str());
		}
	}

	if (unit->isConstructing())
	{
		Unit* targ = unit->getOrderTarget();
		if (targ != NULL)
		{
			Position a = Position(unit->getPosition());
			Position b = Position(targ->getPosition());
			Broodwar->drawLine(CoordinateType::Map,a.x(),a.y(),b.x(),b.y(),Colors::Green);

			Broodwar->drawText(CoordinateType::Map, unit->getPosition().x(), unit->getPosition().y(), "Constructing %s", targ->getType().getName().c_str());
		}
	}
}

void WorkerAgent::computeActions()
{
	if (squadID != -1)
	{
		//Worker is in a squad
		PFManager::getInstance()->computeAttackingUnitActions(this, goal, false);
		return;
	}
	//Check if workers are too far away from a base when attacking
	if (currentState == ATTACKING)
	{
		if (unit->getTarget() != NULL)
		{
			BaseAgent* base = AgentManager::getInstance()->getClosestBase(unit->getTilePosition());
			if (base != NULL)
			{
				double dist = base->getUnit()->getTilePosition().getDistance(unit->getTilePosition());
				if (dist > 25)
				{
					//Stop attacking. Return home
					unit->stop();
					unit->rightClick(base->getUnit());
					setState(GATHER_MINERALS);
					return;
				}
			}
		}
		else
		{
			//No target, return to gather minerals
			setState(GATHER_MINERALS);
			return;
		}
	}

	if (currentState == GATHER_GAS)
	{
		if (unit->isIdle())
		{
			//Not gathering gas. Reset.
			setState(GATHER_MINERALS);
		}
	}
	
	if (currentState == REPAIRING)
	{
		if (!unit->isRepairing())
		{
			setState(GATHER_MINERALS);
			BaseAgent* base = AgentManager::getInstance()->getClosestBase(unit->getTilePosition());
			if (base != NULL)
			{
				unit->rightClick(base->getUnit());
				return;
			}
		}
		else
		{
			return;
		}
	}

	if (currentState == GATHER_MINERALS)
	{
		if (unit->isIdle())
		{
			Unit* mineral = CoverMap::getInstance()->findClosestMineral(unit->getTilePosition());
			if (mineral != NULL)
			{
				unit->rightClick(mineral);
			}
		}
	}

	if (currentState == FIND_BUILDSPOT)
	{
		CoverMap::getInstance()->clearTemp(toBuild, buildSpot);
		buildSpot = CoverMap::getInstance()->findBuildSpot(toBuild);
		if (buildSpot.x() >= 0)
		{
			//Broodwar->printf("[%d] Build spot for %s found at (%d,%d)", Broodwar->getFrameCount(), toBuild.getName().c_str(), buildSpot.x(), buildSpot.y());
			setState(MOVE_TO_SPOT);
			startBuildFrame = Broodwar->getFrameCount();
			if (toBuild.isResourceDepot())
			{
				Commander::getInstance()->updateGoals();
			}
		}
	}

	if (currentState == MOVE_TO_SPOT)
	{
		CoverMap::getInstance()->fillTemp(toBuild, buildSpot);
		if (!buildSpotExplored())
		{
			//Broodwar->printf("[%d] moving to spot (%d,%d) dist=%d", unitID, buildSpot.x(), buildSpot.y());
			unit->rightClick(Position(buildSpot));

			//Broodwar->printf("Move to build %s at (%d,%d)", toBuild.getName().c_str(), buildSpot.x(), buildSpot.y());
		}

		if (buildSpotExplored() && !unit->isConstructing())
		{
			if (areaFree())
			{
				bool ok = unit->build(buildSpot, toBuild);
				if (!ok)
				{
					CoverMap::getInstance()->blockPosition(buildSpot);
					//Cant build at selected spot, get a new one.
					setState(FIND_BUILDSPOT);
				}
			}
			else
			{
				//Cant build at selected spot, get a new one.
				setState(FIND_BUILDSPOT);
			}
		}

		if (unit->isConstructing())
		{
			//Broodwar->printf("[%d] is building at (%d,%d)", unitID, buildSpot.x(), buildSpot.y());
			setState(CONSTRUCT);
			startSpot = TilePosition(-1, -1);
		}
	}

	if (currentState == CONSTRUCT)
	{
		if (isBuilt())
		{
			//Build finished.
			BaseAgent* agent = AgentManager::getInstance()->getClosestBase(unit->getTilePosition());
			if (agent != NULL)
			{
				unit->rightClick(agent->getUnit()->getPosition());
			}
			setState(GATHER_MINERALS);
		}
	}
}

bool WorkerAgent::isBuilt()
{
	for(set<Unit*>::iterator m = Broodwar->getUnitsOnTile(buildSpot.x(), buildSpot.y()).begin(); m != Broodwar->getUnitsOnTile(buildSpot.x(), buildSpot.y()).end(); m++)
	{
		if ((*m)->exists() && (*m)->getPlayer()->getID() == Broodwar->self()->getID())
		{
			if ((*m)->getType().getID() == toBuild.getID())
			{
				return true;
			}
		}
	}
	return false;
}

bool WorkerAgent::areaFree()
{
	if (toBuild.isRefinery())
	{
		return true;
	}

	if (AgentManager::getInstance()->unitsInArea(buildSpot, toBuild.tileWidth(), toBuild.tileHeight(), unit->getID()))
	{
		return false;
	}
	
	return true;
}

bool WorkerAgent::buildSpotExplored()
{
	int sightDist = 64;
	if (toBuild.isRefinery())
	{
		sightDist = 160; //5 tiles
	}

	double dist = unit->getPosition().getDistance(Position(buildSpot));
	//Broodwar->printf("Dist=%d, toReach=%d", (int)dist, sightDist);
	if (dist > sightDist)
	{
		//Broodwar->printf("Not there");
		return false;
	}
	//Broodwar->printf("Arrived");
	return true;
}

int WorkerAgent::getState()
{
	return currentState;
}

void WorkerAgent::setState(int state)
{
	currentState = state;
	
	if (state == GATHER_MINERALS)
	{
		startSpot = TilePosition(-1, -1);
		buildSpot = TilePosition(-1, -1);
	}
}

bool WorkerAgent::assignToRepair(Unit* building)
{
	if (unit->isIdle() || (unit->isGatheringMinerals() && !unit->isCarryingMinerals()))
	{
		setState(REPAIRING);
		unit->repair(building);
		return true;
	}
	return false;
}

bool WorkerAgent::assignToFinishBuild(Unit* building)
{
	if (unit->isIdle() || (unit->isGatheringMinerals() && !unit->isCarryingMinerals()))
	{
		setState(REPAIRING);
		unit->rightClick(building);
		return true;
	}
	return false;
}

bool WorkerAgent::canBuild(UnitType type)
{
	//Make sure we have some spare resources so we dont drain
	//required minerals for our units.
	if (Broodwar->self()->minerals() < type.mineralPrice() + 100)
	{
		return false;
	}

	if (unit->isIdle() || (unit->isGatheringMinerals() && !unit->isCarryingMinerals()))
	{
		if (Broodwar->canMake(unit, type))
		{
			return true;
		}
	}
	return false;
}

bool WorkerAgent::assignToBuild(UnitType type)
{
	toBuild = type;
	buildSpot = CoverMap::getInstance()->findBuildSpot(toBuild);
	if (buildSpot.x() >= 0)
	{
		ResourceManager::getInstance()->lockResources(toBuild);
		setState(FIND_BUILDSPOT);
		return true;
	}
	else
	{
		//Broodwar->printf("No buildspot found for %s", type.getName().c_str());
		startSpot = TilePosition(-1, -1);
		return false;
	}
}

void WorkerAgent::reset()
{
	if (currentState == MOVE_TO_SPOT)
	{
		//The buildSpot is probably not reachable. Block it.	
		CoverMap::getInstance()->blockPosition(buildSpot);
	}

	if (unit->isConstructing())
	{
		unit->cancelConstruction();
	}

	setState(GATHER_MINERALS);
	unit->stop();
	BaseAgent* base = AgentManager::getInstance()->getClosestBase(unit->getTilePosition());
	if (base != NULL)
	{
		unit->rightClick(base->getUnit()->getPosition());
	}
	
	//Broodwar->printf("[%d] worker reset", unitID);
}

bool WorkerAgent::isConstructing(UnitType type)
{
	if (currentState == FIND_BUILDSPOT || currentState == MOVE_TO_SPOT || currentState == CONSTRUCT)
	{
		if (toBuild.getID() == type.getID())
		{
			return true;
		}
	}
	return false;
}

/** Returns the state of the agent as text. Good for printouts. */
string WorkerAgent::getStateAsText()
{
	string strReturn;
	switch(currentState)
	{
	case GATHER_MINERALS:
		strReturn = "GATHER_MINERALS";
		break;
	case GATHER_GAS:
		strReturn = "GATHER_GAS";
		break;
	case FIND_BUILDSPOT:
		strReturn = "FIND_BUILDSPOT";
		break;
	case MOVE_TO_SPOT:
		strReturn = "MOVE_TO_SPOT";
		break;
	case CONSTRUCT:
		strReturn = "CONSTRUCT";
		break;
	case REPAIRING:
		strReturn = "REPAIRING";
		break;
	};
	return strReturn;
}
