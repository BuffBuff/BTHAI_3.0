#include "Commander.h"
#include "AgentManager.h"
#include "ExplorationManager.h"
#include "SpottedObject.h"
#include "SquadFileReader.h"
#include "BuildPlanner.h"
#include "WorkerAgent.h"
#include "Profiler.h"
#include <algorithm>

Commander* Commander::instance = NULL;

Commander::Commander()
{
	currentID = 1;	
	currentState = DEFEND;

	ownDeadScore = 0;
	enemyDeadScore = 0;

	lastCallFrame = Broodwar->getFrameCount();

	SquadFileReader sfr = SquadFileReader();
	squads = sfr.readSquadList();
	sortSquadList();
}

Commander::~Commander()
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		delete squads.at(i);
	}
	delete instance;
}

Commander* Commander::getInstance()
{
	if (instance == NULL)
	{
		instance = new Commander();
	}
	return instance;
}

void Commander::computeActions()
{
	//Dont call too often
	int cFrame = Broodwar->getFrameCount();
	if (cFrame - lastCallFrame < 45)
	{
		return;
	}
	lastCallFrame = cFrame;

	if (currentState == DEFEND)
	{
		if (shallEngage())
		{
			Broodwar->printf("Launch attack");
			forceAttack();
		}
	}

	if (currentState == DEFEND)
	{
		TilePosition defSpot = findChokePoint();
		for (int i = 0; i < (int)squads.size(); i++)
		{
			if (!squads.at(i)->hasGoal())
			{
				if (defSpot.x() != -1)
				{
					squads.at(i)->defend(defSpot);
				}
			}
		}
	}

	if (currentState == ATTACK)
	{
		for (int i = 0; i < (int)squads.size(); i++)
		{
			if (squads.at(i)->isOffensive())
			{
				if (!squads.at(i)->hasGoal())
				{
					TilePosition closeEnemy = getClosestEnemyBuilding(TilePosition(squads.at(i)->getCenter()));
					if (closeEnemy.x() >= 0)
					{
						squads.at(i)->attack(closeEnemy);
					}
				}
			}
			else
			{
				TilePosition defSpot = findChokePoint();
				if (defSpot.x() != -1)
				{
					squads.at(i)->defend(defSpot);
				}
			}
		}
	}
	
	//Check if there are obstacles we can remove. Needed for some
	//strange maps.
	if (Broodwar->getFrameCount() % 150 == 0)
	{
		checkRemovableObstacles();
	}

	//Compute Squad actions.
	for(int i = 0; i < (int)squads.size(); i++)
	{
		squads.at(i)->computeActions();
	}

	//Attack if we have filled all supply spots
	if (currentState == DEFEND)
	{
		int supplyUsed = Broodwar->self()->supplyUsed() / 2;
		if (supplyUsed >= 198)
		{
			forceAttack();
		}
	}

	//Terran only: Check for repairs and finish unfinished buildings
	if (BuildPlanner::isTerran())
	{
		//Check if there are unfinished buildings we need
		//to complete.
		checkUnfinishedBuildings();

		//Check if there are units we need to repair.
		checkRepairUnits();
	}

	//Check for units not belonging to a squad
	checkNoSquadUnits();
}

bool Commander::shallEngage()
{
	TilePosition closeEnemy = getClosestEnemyBuilding(Broodwar->self()->getStartLocation());
	if (closeEnemy.x() == -1)
	{
		//No enemy sighted. Dont launch attack.
		return false;
	}

	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (squads.at(i)->isRequired() && !squads.at(i)->isActive())
		{
			return false;
		}
	}
	return true;
}

void Commander::updateGoals()
{
	TilePosition defSpot = findChokePoint();
	
	if (defSpot.x() != -1)
	{
		for (int i = 0; i < (int)squads.size(); i++)
		{
			Squad* sq = squads.at(i);
			squads.at(i)->defend(defSpot);
		}
	}
}

void Commander::debug_showGoal()
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		squads.at(i)->debug_showGoal();
	}
}

bool Commander::isLaunchingAttack()
{
	if (currentState == ATTACK)
	{
		return true;
	}
	return false;
}

void Commander::checkNoSquadUnits()
{
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		
		bool notAssigned = true;
		if (!agent->isAlive()) notAssigned = false;
		if (agent->getUnitType().isWorker()) notAssigned = false;
		if (!agent->getUnit()->exists()) notAssigned = false;
		if (agent->isOfType(UnitTypes::Zerg_Overlord)) notAssigned = false;
		if (agent->getUnitType().isBuilding()) notAssigned = false;
		if (agent->getUnitType().isAddon()) notAssigned = false;
		if (agent->getSquadID() != -1) notAssigned = false;

		if (notAssigned)
		{
			assignUnit(agent);
		}
	}
}

void Commander::assignUnit(BaseAgent* agent)
{
	//Broodwar->printf("%s (%s) is not assigned to a squad", agent->getUnitType().getName().c_str(), agent->getTypeName().c_str());

	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		if (sq->needUnit(agent->getUnitType()))
		{
			sq->addMember(agent);
			//Broodwar->printf("%s is assigned to SQ %d", agent->getUnitType().getName().c_str(), sq->getID());
			return;
		}
	}
}

TilePosition Commander::getClosestEnemyBuilding(TilePosition start)
{
	Unit* closestBuilding = NULL;
	SpottedObject* closestObj = NULL;
	double bestDist = -1;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->getType().isBuilding())
		{
			double cDist = start.getDistance((*i)->getTilePosition());
			
			if (!ExplorationManager::canReach(start, (*i)->getTilePosition()))
			{
				//cDist = -1;
			}

			if (bestDist < 0 || cDist < bestDist)
			{
				bestDist = cDist;
				closestBuilding = (*i);
			}
		}
	}

	if (closestBuilding != NULL)
	{
		//Broodwar->printf("[TSC]: Closest enemy building is %s", closestBuilding->getType().getName().c_str());
		return closestBuilding->getTilePosition();
	}

	if (ExplorationManager::getInstance()->isActive())
	{
		bestDist = -1;
		vector<SpottedObject*> units = ExplorationManager::getInstance()->getSpottedBuildings();
		for (int i = 0; i < (int)units.size(); i++)
		{
			SpottedObject* obj = units.at(i);
			if (obj->getType().isBuilding() && obj->isActive())
			{
				double cDist = obj->getDistance(start);
				if (bestDist < 0 || cDist < bestDist)
				{
					bestDist = cDist;
					closestObj = obj;
				}
			}
		}

		if (closestObj != NULL)
		{
			//Broodwar->printf("[TSC]: Closest enemy building is %s", closestBuilding->getType().getName().c_str());
			return closestObj->getTilePosition();
		}
	}

	//No building has been spotted. Move towards a starting point.
	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (squads.at(i)->isOffensive() && squads.at(i)->getSize() > 0)
		{
			TilePosition nextArea = squads.at(i)->getNextStartLocation();
			return nextArea;
		}
	}
	
	return TilePosition(-1,-1);
}

void Commander::handleCloakedEnemy(TilePosition pos, Squad* squad)
{
	if (BuildPlanner::isProtoss())
	{
		
	}
	if (BuildPlanner::isTerran())
	{
		vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
		if ((int)agents.size() > 0)
		{
			bool ok = agents.at(0)->doScannerSweep(pos);
			if (ok)
			{
				return;
			}
		}
	}
	if (BuildPlanner::isZerg())
	{
		
	}
}

Squad* Commander::getSquad(int id)
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (squads.at(i)->getID() == id)
		{
			return squads.at(i);
		}
	}
	return NULL;
}

vector<Squad*> Commander::getSquads()
{
	return squads;
}

void Commander::unitDestroyed(BaseAgent* agent)
{
	int squadID = agent->getSquadID();
	if (squadID != -1)
	{
		Squad* squad = getSquad(squadID);
		if (squad != NULL)
		{
			squad->removeMember(agent);
		}
	}
}

void Commander::sortSquadList()
{
	sort(squads.begin(), squads.end(), SortSquadList());
}

void Commander::unitCreated(BaseAgent* agent)
{
	//Sort the squad list
	sortSquadList();

	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (squads.at(i)->addMember(agent))
		{
			break;
		}
	}
}

void Commander::checkRemovableObstacles()
{
	for(set<Unit*>::iterator m = Broodwar->getMinerals().begin(); m != Broodwar->getMinerals().end(); m++)
	{
		if ((*m)->getResources() <= 20)
		{
			//Found a mineral that we can remove.
			BaseAgent* baseAgent = AgentManager::getInstance()->getClosestBase((*m)->getTilePosition());
			if (baseAgent != NULL)
			{
				double cDist = baseAgent->getUnit()->getDistance((*m));
				if (cDist < 1000)
				{
					//It is close to a base, remove it.

					//Step 1: Check if someone already is working on it
					bool assign = true;
					vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
					for (int i = 0; i < (int)agents.size(); i++)
					{
						BaseAgent* agent = agents.at(i);
						if (agent->isWorker())
						{
							Unit* u = agent->getUnit();
							if (u->isGatheringMinerals())
							{
								Unit* t = u->getTarget();
								if (t != NULL && t->getID() == u->getID())
								{
									//Someone already working on it. Dont bother.
									assign = false;
									break;
								}
							}
						}
					}

					if (assign)
					{
						BaseAgent* worker = AgentManager::getInstance()->findClosestFreeWorker((*m)->getTilePosition());
						if (worker != NULL)
						{
							worker->getUnit()->rightClick((*m));
						}
					}
				}
			}
		}
	}
}

TilePosition Commander::findUnfortifiedChokePoint()
{
	double bestDist = 0;
	Chokepoint* bestChoke = NULL;

	for(set<Region*>::const_iterator i=getRegions().begin();i!=getRegions().end();i++)
	{
		if (isOccupied((*i)))
		{
			for(set<Chokepoint*>::const_iterator c=(*i)->getChokepoints().begin();c!=(*i)->getChokepoints().end();c++)
			{
				if (isEdgeChokepoint((*c)))
				{
					if (!chokePointFortified(TilePosition((*c)->getCenter())))
					{
						double cDist = Broodwar->self()->getStartLocation().getDistance(TilePosition((*c)->getCenter()));
						if (cDist > bestDist)
						{
							bestDist = cDist;
							bestChoke = (*c);
						}
					}
				}
			}
		}
	}

	TilePosition buildPos = TilePosition(-1, -1);
	if (bestChoke != NULL)
	{
		buildPos = TilePosition(bestChoke->getCenter());
	}
	return buildPos;
}

bool Commander::chokePointFortified(TilePosition center)
{
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if (agents.at(i)->isOfType(UnitTypes::Terran_Bunker) || agents.at(i)->isOfType(UnitTypes::Protoss_Photon_Cannon) || agents.at(i)->isOfType(UnitTypes::Zerg_Sunken_Colony) || agents.at(i)->isOfType(UnitTypes::Zerg_Creep_Colony) || agents.at(i)->isOfType(UnitTypes::Zerg_Spore_Colony))
			{
				double dist = center.getDistance(agents.at(i)->getUnit()->getTilePosition());
				if (dist <= 20)
				{
					return true;
				}
			}
		}
	}
	return false;
}

double Commander::getChokepointPrio(TilePosition center)
{
	TilePosition ePos = ExplorationManager::getInstance()->getClosestSpottedBuilding(center);

	if (ePos.x() >= 0)
	{
		double dist = ePos.getDistance(center);
		return 1000 - dist;
	}
	else
	{
		double dist = Broodwar->self()->getStartLocation().getDistance(center);
		return dist;
	}
}

TilePosition Commander::findChokePoint()
{
	double bestPrio = -1;
	Chokepoint* bestChoke = NULL;
	
	for(set<Region*>::const_iterator i=getRegions().begin();i!=getRegions().end();i++)
	{
		if (isOccupied((*i)))
		{
			for(set<Chokepoint*>::const_iterator c=(*i)->getChokepoints().begin();c!=(*i)->getChokepoints().end();c++)
			{
				if (isEdgeChokepoint((*c)))
				{
					double cPrio = getChokepointPrio(TilePosition((*c)->getCenter()));
					if (cPrio > bestPrio)
					{
						bestPrio = cPrio;
						bestChoke = (*c);
					}
				}
			}
		}
	}

	TilePosition guardPos = Broodwar->self()->getStartLocation();
	if (bestChoke != NULL)
	{
		guardPos = findDefensePos(bestChoke);
	}

	return guardPos;
}

TilePosition Commander::findDefensePos(Chokepoint* choke)
{
	TilePosition defPos = TilePosition(choke->getCenter());
	TilePosition chokePos = defPos;

	double size = choke->getWidth();
	if (size <= 32 * 3)
	{
		//Very narrow chokepoint, dont crowd it
		double bestDist = 1000;
		TilePosition basePos = Broodwar->self()->getStartLocation();

		int maxD = 8;
		int minD = 5;

		//We found a chokepoint. Now we need to find a good place to defend it.
		for (int cX = chokePos.x() - maxD; cX <= chokePos.x() + maxD; cX++)
		{
			for (int cY = chokePos.y() - maxD; cY <= chokePos.y() + maxD; cY++)
			{
				TilePosition cPos = TilePosition(cX, cY);
				if (ExplorationManager::canReach(basePos, cPos))
				{
					double chokeDist = chokePos.getDistance(cPos);
					double baseDist = basePos.getDistance(cPos);

					if (chokeDist >= minD && chokeDist <= maxD)
				{
						if (baseDist < bestDist)
				{
							bestDist = baseDist;
							defPos = cPos;
						}
					}
				}
			}
		}
	}

	return defPos;
}

bool Commander::isEdgeChokepoint(Chokepoint* choke)
{
	pair<Region*,Region*> regions = choke->getRegions();
	if (isOccupied(regions.first) && isOccupied(regions.second))
	{
		return false;
	}
	return true;
}

bool Commander::isOccupied(Region* region)
{
	BWTA::Polygon p = region->getPolygon();
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive() && agent->isBuilding())
		{
			Region* aRegion = getRegion(agents.at(i)->getUnit()->getTilePosition());
			Position c1 = region->getCenter();
			Position c2 = aRegion->getCenter();
			if (c2.x() == c1.x() && c2.y() == c1.y())
			{
				return true;
			}
		}
	}

	//Check expansion site
	TilePosition expansionSite = ExplorationManager::getInstance()->getExpansionSite();
	TilePosition center = TilePosition(region->getCenter());
	if (expansionSite.x() >= 0)
	{
		double dist = expansionSite.getDistance(center);
		if (dist <= 15)
		{
			return true;
		}
	}

	return false;
}

bool Commander::chokePointGuarded(TilePosition center)
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		if (sq->isDefensive() || sq->isOffensive())
		{
			double dist = center.getDistance(sq->getCenter());
			if (dist <= 5)
			{
				return true;
			}
			dist = center.getDistance(sq->getGoal());
			if (dist <= 5)
			{
				return true;
			}
		}
	}

	return false;
}

bool Commander::isPowered(TilePosition pos)
{
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isOfType(UnitTypes::Protoss_Pylon))
		{
			double dist = agent->getUnit()->getTilePosition().getDistance(pos);
			if (dist <= 4)
			{
				return true;
			}
		}
	}
	return false;
}

bool Commander::isBuildable(TilePosition pos)
{
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isOfType(UnitTypes::Protoss_Pylon))
		{
			if (!agent->getUnit()->isBeingConstructed())
			{
				double dist = agent->getUnit()->getTilePosition().getDistance(pos);
				if (dist <= 4)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool Commander::needUnit(UnitType type)
{
	int prevPrio = 1000;

	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (!squads.at(i)->isFull())
		{
			if (squads.at(i)->getPriority() > prevPrio)
			{
				return false;
			}

			if (squads.at(i)->needUnit(type))
			{
				return true;
			}
			
			prevPrio = squads.at(i)->getPriority();
		}
	}
	return false;
}

TilePosition Commander::findOffensiveSquadPosition(TilePosition closeEnemy)
{
	TilePosition pos = TilePosition(-1, -1);

	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		if (pos.x() == -1)
		{
			if (sq->isActive() && sq->isOffensive() && sq->isGround() && currentState == ATTACK && sq->size() >= 2)
			{
				pos = sq->getCenter();
			}
		}
		if (sq->isAttacking() && sq->isOffensive())
		{
			return closeEnemy;
		}
	}

	if (pos.x() == -1)
	{
		//No position found, wait at home base
		pos = Broodwar->self()->getStartLocation();
	}

	return pos;
}

void Commander::assistBuilding(BaseAgent* building)
{
	//Find out who targets the building
	TilePosition defPos = building->getUnit()->getTilePosition();
	Unit* target = NULL;
	int bestScore = -1;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			double dist = (*i)->getDistance(Position(defPos));
			if (dist <= 13 * 32)
			{
				//Found a target
				if ((*i)->getType().destroyScore() > bestScore)
				{
					target = (*i);
					bestScore = (*i)->getType().destroyScore();
				}
			}
		}
	}

	if (target != NULL)
	{
		//Broodwar->printf("Assisting building: Targeting enemy %s", target->getType().getName().c_str());
		defPos = target->getTilePosition();
	}
	
	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		if (sq->isDefensive() || sq->isOffensive() || sq->isSupport())
		{
			sq->assist(defPos);
		}
	}
}

void Commander::assistWorker(BaseAgent* worker)
{
	if (worker->getSquadID() != -1)
	{
		//Worker is in a squad. Do nothing.
		return;
	}

	//Find out who targets the worker
	TilePosition defPos = worker->getUnit()->getTilePosition();
	Unit* target = NULL;
	int bestScore = -1;

	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		if ((*i)->exists())
		{
			double dist = (*i)->getDistance(Position(defPos));
			if (dist <= 13 * 32)
			{
				//Found a target
				if ((*i)->getType().destroyScore() > bestScore)
				{
					target = (*i);
					bestScore = (*i)->getType().destroyScore();
				}
			}
		}
	}

	if (target != NULL)
	{
		//Broodwar->printf("Assisting building: Targeting enemy %s", target->getType().getName().c_str());
		defPos = target->getTilePosition();
	}
	
	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		if (sq->isDefensive() || sq->isOffensive() || sq->isSupport())
		{
			sq->assist(defPos);
		}
	}
}

void Commander::forceAttack()
{
	TilePosition cGoal = getClosestEnemyBuilding(Broodwar->self()->getStartLocation());
	
	Broodwar->printf("Force attack at (%d,%d)", cGoal.x(), cGoal.y());

	if (cGoal.x() == -1)
	{
		return;
	}

	for (int i = 0; i < (int)squads.size(); i++)
	{
		if (squads.at(i)->isOffensive() || squads.at(i)->isSupport())
		{
			if (cGoal.x() >= 0)
			{
				squads.at(i)->forceActive();
				squads.at(i)->attack(cGoal);
			}
		}
	}

	currentState = ATTACK;
}

bool Commander::checkRepairUnits()
{
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++) 
	{
		BaseAgent* agent = agents.at(i);
		if (agent != NULL && agent->isAlive() && isImportantUnit(agent->getUnit()) && agent->isDamaged() && !agent->getUnit()->isBeingConstructed())
		{
			repair(agent);
		}
	}
	return false;
}

bool Commander::isImportantUnit(Unit* unit)
{
	UnitType type = unit->getType();

	if (type.isBuilding()) return true;
	if (BaseAgent::isOfType(type, UnitTypes::Terran_Siege_Tank_Tank_Mode)) return true;
	if (BaseAgent::isOfType(type, UnitTypes::Terran_Siege_Tank_Siege_Mode)) return true;
	if (BaseAgent::isOfType(type, UnitTypes::Terran_Science_Vessel)) return true;
	if (BaseAgent::isOfType(type, UnitTypes::Terran_Battlecruiser)) return true;

	return false;
}

void Commander::repair(BaseAgent* agent)
{
	
	//First we must check if someone is repairing this building
	if(AgentManager::getInstance()->isAnyAgentRepairingThisAgent(agent))
	{
		return;
	}

	BaseAgent* repUnit = AgentManager::getInstance()->findClosestFreeWorker(agent->getUnit()->getTilePosition());
	if (repUnit != NULL)
	{
		//Broodwar->printf("Assigned SCV %d to repair %s (%d/%d)", repUnit->getUnitID(), agent->getUnitType().getName().c_str(), agent->getUnit()->getHitPoints(), agent->getUnitType().maxHitPoints());
		repUnit->assignToRepair(agent->getUnit());
	}
}

void Commander::finishBuild(BaseAgent* baseAgent)
{
	//First we must check if someone is repairing this building
	if(AgentManager::getInstance()->isAnyAgentRepairingThisAgent(baseAgent))
		return;

	BaseAgent* repUnit = AgentManager::getInstance()->findClosestFreeWorker(baseAgent->getUnit()->getTilePosition());
	if (repUnit != NULL)
	{
		repUnit->assignToFinishBuild(baseAgent->getUnit());
	}
}

bool Commander::checkUnfinishedBuildings()
{
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		// Double check so we don't get a dead agent. This could cause null pointers.
		if (agent != NULL && agent->isAlive() && agent->isBuilding() && agent->isDamaged())
		{
			Unit* builder = agent->getUnit()->getBuildUnit();
			if (builder == NULL || !builder->isConstructing())
			{
				finishBuild(agent);
			}
		}
	}
	return false;
}

void Commander::printInfo()
{
	Broodwar->drawTextScreen(295,0,"Attacking squads:");
	int no = 0;
	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		if (sq->isRequired())
		{
			Broodwar->drawTextScreen(295,no*16+16, "SQ %d: (%d/%d)", sq->getID(), sq->getSize(), sq->getTotalUnits());
			no++;
		}
	}

	//Broodwar->drawTextScreen(440,32, "Own Deadscore: %d", ownDeadScore);
	//Broodwar->drawTextScreen(440,48, "Enemy Deadscore: %d", enemyDeadScore);
}

void Commander::addBunkerSquad()
{
	Squad* bSquad = new Squad(100 + AgentManager::getInstance()->countNoUnits(UnitTypes::Terran_Bunker), Squad::BUNKER, "BunkerSquad", 5);
	bSquad->addSetup(UnitTypes::Terran_Marine, 4);
	squads.push_back(bSquad);

	//Try to fill from other squads.
	int added = 0;
	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		if (sq->isOffensive() || sq->isDefensive())
		{
			for (int i = 0; i < 4 - added; i++)
			{
				if (sq->hasUnits(UnitTypes::Terran_Marine, 1))
				{
					if (added < 4)
				{
						BaseAgent* ma = sq->removeMember(UnitTypes::Terran_Marine);
						if (ma != NULL)
				{
							added++;
							bSquad->addMember(ma);
							ma->clearGoal();
						}
					}
				}
			}
		}
	}
}
