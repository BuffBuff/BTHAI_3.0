#include "ExplorationManager.h"
#include "AgentManager.h"
#include "VultureAgent.h"
#include "BaseAgent.h"
#include "Commander.h"
#include "Squad.h"
#include "CoverMap.h"

bool ExplorationManager::instanceFlag = false;
ExplorationManager* ExplorationManager::instance = NULL;

ExplorationManager::ExplorationManager()
{
	active = true;
	
	ownForce.reset();
	enemyForce.reset();

	//Add the regions for this map
	for(set<Region*>::const_iterator i=getRegions().begin();i!=getRegions().end();i++)
	{
		exploreData.push_back(ExploreData((*i)->getCenter()));
	}

	siteSetFrame = 0;

	lastCallFrame = Broodwar->getFrameCount();

	expansionSite = TilePosition(-1, -1);
}

ExplorationManager::~ExplorationManager()
{
	
	for (int i = 0; i < (int)spottedBuildings.size(); i++)
	{
		//delete spottedBuildings.at(i);
	}
	
	instanceFlag = false;
	delete instance;
}

void ExplorationManager::setInactive()
{
	active = false;
}

bool ExplorationManager::isActive()
{
	return active;
}

ExplorationManager* ExplorationManager::getInstance()
{
	if (!instanceFlag)
	{
		instance = new ExplorationManager();
		instanceFlag = true;
	}
	return instance;
}

void ExplorationManager::computeActions()
{
	//Dont call too often
	int cFrame = Broodwar->getFrameCount();
	if (cFrame - lastCallFrame < 73)
	{
		return;
	}
	lastCallFrame = cFrame;

	if (!active)
	{
		return;
	}

	if (Broodwar->getFrameCount() % 200 == 0)
	{
		calcEnemyForceData();
		calcOwnForceData();
	}
}

TilePosition ExplorationManager::searchExpansionSite()
{
	getExpansionSite();

	if (expansionSite.x() == -1)
	{
		expansionSite = CoverMap::getInstance()->findExpansionSite();
		siteSetFrame = Broodwar->getFrameCount();
		//Broodwar->printf("Found expansion site around (%d,%d)", expansionSite.x(), expansionSite.y());
	}

	return expansionSite;
}

TilePosition ExplorationManager::getExpansionSite()
{
	if (expansionSite.x() >= 0)
	{
		if (Broodwar->getFrameCount() - siteSetFrame > 500)
		{
			expansionSite = TilePosition(-1, -1);
		}
	}

	return expansionSite;
}

void ExplorationManager::setExpansionSite(TilePosition pos)
{
	if (pos.x() >= 0)
	{
		siteSetFrame = Broodwar->getFrameCount();
		expansionSite = pos;
	}
}

TilePosition ExplorationManager::getNextToExplore(Squad* squad)
{
	TilePosition curPos = squad->getCenter();
	TilePosition goal = squad->getGoal();

	//Special case: No goal set
	if (goal.x() == -1 || goal.y() == -1)
	{
		Region* startRegion = getRegion(curPos); 
		goal = TilePosition(startRegion->getCenter());
		return goal;
	}

	double dist = curPos.getDistance(goal);

	double acceptDist = 4;
	if (squad->isGround())
	{
		acceptDist = 6;
	}

	if (dist <= acceptDist)
	{
		//Squad is close to goal

		//1. Set region to explored
		setExplored(goal);

		//2. Find new region to explore
		Region* startRegion = getRegion(goal);
		Region* bestRegion = startRegion;

		if (bestRegion != NULL)
		{
			int bestLastVisitFrame = getLastVisitFrame(bestRegion);

			if (!squad->isAir())
			{
				//Ground explorers
				for(set<Region*>::const_iterator i=startRegion->getReachableRegions().begin();i!=startRegion->getReachableRegions().end();i++)
				{
					int cLastVisitFrame = getLastVisitFrame((*i));
					TilePosition c = TilePosition((*i)->getCenter());
					if (cLastVisitFrame <= bestLastVisitFrame)
				{
						bestLastVisitFrame = cLastVisitFrame;
						bestRegion = (*i);
					}
				}
			}
			else
			{
				//Air explorers
				double bestDist = 100000;
				for(set<Region*>::const_iterator i=getRegions().begin();i!=getRegions().end();i++)
				{
					int cLastVisitFrame = getLastVisitFrame((*i));
					TilePosition c = TilePosition((*i)->getCenter());
					double dist = c.getDistance(curPos);
					if (cLastVisitFrame < bestLastVisitFrame)
				{
						bestLastVisitFrame = cLastVisitFrame;
						bestRegion = (*i);
						bestDist = dist;
					}
					if (cLastVisitFrame == bestLastVisitFrame && dist < bestDist)
				{
						bestLastVisitFrame = cLastVisitFrame;
						bestRegion = (*i);
						bestDist = dist;
					}
				}
			}

			TilePosition newGoal = TilePosition(bestRegion->getCenter());
			return newGoal;
			//Broodwar->printf("Explorer: new goal (%d,%d) I am at (%d,%d) agentGoal (%d,%d)", newGoal.x(), newGoal.y(), curPos.x(), curPos.y(), agent->getGoal().x(), agent->getGoal().y());
		}
	}

	return TilePosition(-1, -1);
}

void ExplorationManager::setExplored(TilePosition goal)
{
	bool found = false;
	for (int i = 0; i < (int)exploreData.size(); i++)
	{
		if (exploreData.at(i).matches(goal))
		{
			exploreData.at(i).lastVisitFrame = Broodwar->getFrameCount();
			found = true;
		}
	}
}

int ExplorationManager::getLastVisitFrame(Region* region)
{
	for (int i = 0; i < (int)exploreData.size(); i++)
	{
		if (exploreData.at(i).matches(region))
		{

			//Check if region is visible. If so, set lastVisitFrame to now
			if (Broodwar->isVisible(exploreData.at(i).center))
			{
				exploreData.at(i).lastVisitFrame = Broodwar->getFrameCount();
			}

			return exploreData.at(i).lastVisitFrame;
		}
	}
	
	//Error: No region found
	TilePosition goal = TilePosition(region->getCenter());
	Broodwar->printf("FATAL GetLastVF: Unable to find region for tile (%d,%d)", goal.x(), goal.y());
	return -1;
}

void ExplorationManager::showIntellData()
{
	Broodwar->drawTextScreen(250,16*2, "AirAttackStr: %d (%d)", enemyForce.airAttackStr, ownForce.airAttackStr);
	Broodwar->drawTextScreen(250,16*3, "AirDefendStr: %d (%d)", enemyForce.airDefendStr, ownForce.airDefendStr);
	Broodwar->drawTextScreen(250,16*4, "GroundAttackStr: %d (%d)", enemyForce.groundAttackStr, ownForce.groundAttackStr);
	Broodwar->drawTextScreen(250,16*5, "GroundDefendStr: %d (%d)", enemyForce.groundDefendStr, ownForce.groundDefendStr);

	Broodwar->drawTextScreen(250,16*6, "CommandCenters: %d (%d)", enemyForce.noCommandCenters, ownForce.noCommandCenters);
	Broodwar->drawTextScreen(250,16*7, "Factories: %d (%d)", enemyForce.noFactories, ownForce.noFactories);
	Broodwar->drawTextScreen(250,16*8, "Airports: %d (%d)", enemyForce.noAirports, ownForce.noAirports);
	Broodwar->drawTextScreen(250,16*9, "DefenseStructures: %d (%d)", enemyForce.noDefenseStructures, ownForce.noDefenseStructures);
	Broodwar->drawTextScreen(250,16*10, "DetectorStructures: %d (%d)", enemyForce.noDetectorStructures, ownForce.noDetectorStructures);
	Broodwar->drawTextScreen(250,16*11, "DetectorUnits: %d (%d)", enemyForce.noDetectorUnits, ownForce.noDetectorUnits);
}

void ExplorationManager::calcOwnForceData()
{
	ownForce.reset();

	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			UnitType type = agents.at(i)->getUnitType();
			if (type.canAttack() && !type.isWorker())
			{
				if (!type.isBuilding())
				{
					if (type.isFlyer())
				{
						ownForce.airAttackStr += type.destroyScore();
					}
					else
				{
						ownForce.groundAttackStr += type.destroyScore();
					}
				}

				if (UnitAgent::getAirRange(type) >= 0)
				{
					ownForce.airDefendStr += type.destroyScore();
				}
				if (UnitAgent::getGroundRange(type) >= 0)
				{
					ownForce.groundDefendStr += type.destroyScore();
				}
			}

			ownForce.checkType(type);
		}
	}
}

void ExplorationManager::calcEnemyForceData()
{
	enemyForce.reset();

	for (int i = 0; i < (int)spottedUnits.size(); i++)
	{
		if (spottedUnits.at(i)->isActive())
		{
			UnitType type = spottedUnits.at(i)->getType();
			if (type.canAttack() && !type.isWorker())
			{
				if (!type.isBuilding())
				{
					if (type.isFlyer())
				{
						enemyForce.airAttackStr += type.destroyScore();
					}
					else
				{
						enemyForce.groundAttackStr += type.destroyScore();
					}
				}

				if (UnitAgent::getAirRange(type) >= 0)
				{
					enemyForce.airDefendStr += type.destroyScore();
				}
				if (UnitAgent::getGroundRange(type) >= 0)
				{
					enemyForce.groundDefendStr += type.destroyScore();
				}
			}

			enemyForce.checkType(type);
		}
	}

	for (int i = 0; i < (int)spottedBuildings.size(); i++)
	{
		if (spottedBuildings.at(i)->isActive() && spottedBuildings.at(i)->getUnitID() != 10101)
		{
			UnitType type = spottedBuildings.at(i)->getType();
			enemyForce.checkType(type);
		}
	}
}

void ExplorationManager::printInfo()
{
	//Uncomment this if you want to draw a mark at detected enemy buildings.
	/*for (int i = 0; i < (int)spottedBuildings.size(); i++)
	{
		if (spottedBuildings.at(i)->isActive())
		{
			int x1 = spottedBuildings.at(i)->getTilePosition().x() * 32;
			int y1 = spottedBuildings.at(i)->getTilePosition().y() * 32;
			int x2 = x1 + 32;
			int y2 = y1 + 32;

			Broodwar->drawBox(CoordinateType::Map,x1,y1,x2,y2,Colors::Blue,true);
		}
	}*/

	//Draw a circle around detectors
}

void ExplorationManager::addSpottedUnit(Unit* unit)
{
	if (unit->getType().isBuilding())
	{
		
		//Check if we already have seen this building
		bool found = false;
		for (int i = 0; i < (int)spottedBuildings.size(); i++)
		{
			if (spottedBuildings.at(i)->getUnitID() == unit->getID())
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			//Broodwar->printf("[EM] Enemy %s spotted at (%d,%d)", unit->getType().getName().c_str(), unit->getPosition().x(), unit->getPosition().y());
			spottedBuildings.push_back(new SpottedObject(unit));
		}
	}
	else
	{
		bool found = false;
		for (int i = 0; i < (int)spottedUnits.size(); i++)
		{
			if (spottedUnits.at(i)->getUnitID() == unit->getID())
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			spottedUnits.push_back(new SpottedObject(unit));
		}
	}
}

void ExplorationManager::unitDestroyed(Unit* unit)
{
	TilePosition uPos = unit->getTilePosition();
	if (unit->getType().isBuilding())
	{
		bool removed = false;
		for (int i = 0; i < (int)spottedBuildings.size(); i++)
		{
			TilePosition sPos = spottedBuildings.at(i)->getTilePosition();
			if (uPos.x() == sPos.x() && uPos.y() == sPos.y())
			{
				spottedBuildings.at(i)->setInactive();
				removed = true;
			}
		}

		if (!removed)
		{
			//Broodwar->printf("[EM]: Building %s at (%d,%d) was not removed from EM!!!", unit->getType().getName().c_str(), uPos.x(), uPos.y());
		}
	}
	else
	{
		for (int i = 0; i < (int)spottedUnits.size(); i++)
		{
			if (spottedUnits.at(i)->getUnitID() == unit->getID())
			{
				spottedUnits.at(i)->setInactive();
				//Broodwar->printf("[EM]: Remove %s at (%d,%d)", unit->getType().getName().c_str(), uPos.x(), uPos.y());
			}
		}
	}
}

void ExplorationManager::cleanup()
{
	for (int i = 0; i < (int)spottedBuildings.size(); i++)
	{
		if (spottedBuildings.at(i)->isActive())
		{
			if (Broodwar->isVisible(spottedBuildings.at(i)->getTilePosition()))
			{
				int id = spottedBuildings.at(i)->getUnitID();
				bool found = false;
				for(set<Unit*>::const_iterator it=Broodwar->enemy()->getUnits().begin();it!=Broodwar->enemy()->getUnits().end();it++)
				{
					if ((*it)->exists())
				{
						if ((*it)->getID() == id)
				{
							found = true;
							break;
						}
					}
				}
				if (!found)
				{
					spottedBuildings.at(i)->setInactive();
				}
			}
		}
	}
}

int ExplorationManager::spottedBuildingsWithinRange(TilePosition pos, int range)
{
	cleanup();

	int eCnt = 0;
	for (int i = 0; i < (int)spottedBuildings.size(); i++)
	{
		if (spottedBuildings.at(i)->isActive())
		{
			if (pos.getDistance(spottedBuildings.at(i)->getTilePosition()) <= range)
			{
				eCnt++;
			}
		}
	}

	return eCnt;
}

TilePosition ExplorationManager::getClosestSpottedBuilding(TilePosition start)
{
	cleanup();

	TilePosition pos = TilePosition(-1, -1);
	double bestDist = 100000;

	for (int i = 0; i < (int)spottedBuildings.size(); i++)
	{
		if (spottedBuildings.at(i)->isActive())
		{
			double cDist = start.getDistance(spottedBuildings.at(i)->getTilePosition());
			if (cDist < bestDist)
			{
				bestDist = cDist;
				pos = spottedBuildings.at(i)->getTilePosition();
			}
		}
	}

	return pos;
}

vector<SpottedObject*> ExplorationManager::getSpottedBuildings()
{
	cleanup();
	return spottedBuildings;
}

bool ExplorationManager::buildingsSpotted()
{
	if (spottedBuildings.size() > 0)
	{
		return true;
	}
	return false;
}

bool ExplorationManager::canReach(TilePosition a, TilePosition b)
{
	int w = Broodwar->mapWidth();
	int h = Broodwar->mapHeight();
	if (a.x() < 0 || a.x() >= w || a.y() < 0 || a.y() >= h)
	{
		return false;
	}
	if (b.x() < 0 || b.x() >= w || b.y() < 0 || b.y() >= h)
	{
		return false;
	}
	bool ok = true;

	ok = a.hasPath(b);
	
	return ok;
}

bool ExplorationManager::canReach(BaseAgent* agent, TilePosition b)
{
	return agent->getUnit()->hasPath(Position(b));
}

bool ExplorationManager::enemyIsProtoss()
{
	for(set<Player*>::const_iterator i=Broodwar->getPlayers().begin();i!=Broodwar->getPlayers().end();i++)
	{
		if ((*i)->isEnemy(Broodwar->self()))
		{
			if ((*i)->getRace().getID() == Races::Protoss.getID())
			{
				return true;
			}
		}
	}
	return false;
}

bool ExplorationManager::enemyIsZerg()
{
	for(set<Player*>::const_iterator i=Broodwar->getPlayers().begin();i!=Broodwar->getPlayers().end();i++)
	{
		if ((*i)->isEnemy(Broodwar->self()))
		{
			if ((*i)->getRace().getID() == Races::Zerg.getID())
			{
				return true;
			}
		}
	}
	return false;
}

bool ExplorationManager::enemyIsTerran()
{
	for(set<Player*>::const_iterator i=Broodwar->getPlayers().begin();i!=Broodwar->getPlayers().end();i++)
	{
		if ((*i)->isEnemy(Broodwar->self()))
		{
			if ((*i)->getRace().getID() == Races::Terran.getID())
			{
				return true;
			}
		}
	}
	return false;
}

bool ExplorationManager::enemyIsUnknown()
{
	if (!enemyIsTerran() && !enemyIsProtoss() && !enemyIsZerg())
	{
		return true;
	}
	return false;
}

TilePosition ExplorationManager::scanForVulnerableBase()
{
	TilePosition spot = TilePosition(-1, -1);
	for (int i = 0; i < (int)spottedBuildings.size(); i++)
	{
		if (spottedBuildings.at(i)->isActive())
		{
			SpottedObject* obj = spottedBuildings.at(i);
			if (obj->getType().isResourceDepot())
			{
				if (!isDetectorCovering(obj->getTilePosition()))
				{
					//Broodwar->printf("Found probable vulnerable base at (%d,%d)", obj->getTilePosition().x(), obj->getTilePosition().y());
					spot = obj->getTilePosition();
				}
			}
		}
	}

	if (spot.x() < 0)
	{
		//Broodwar->printf("Scan: No vulnerable base found");
	}

	return spot;
}

bool ExplorationManager::isDetectorCovering(TilePosition pos)
{
	return isDetectorCovering(Position(pos));
}

bool ExplorationManager::isDetectorCovering(Position pos)
{
	for (int i = 0; i < (int)spottedBuildings.size(); i++)
	{
		if (spottedBuildings.at(i)->isActive())
		{
			SpottedObject* obj = spottedBuildings.at(i);
			if (obj->getType().isDetector())
			{
				double dist = obj->getPosition().getDistance(pos);
				if (dist <= obj->getType().sightRange())
				{
					return true;
				}
			}
		}
	}
	return false;
}
