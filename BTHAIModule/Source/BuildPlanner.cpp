#include "BuildPlanner.h"
#include "WorkerAgent.h"
#include "StructureAgent.h"
#include "BaseAgent.h"
#include "AgentManager.h"
#include "CoverMap.h"
#include "UnitSetup.h"
#include "ExplorationManager.h"
#include "BuildOrderFileReader.h"
#include "ResourceManager.h"

BuildPlanner* BuildPlanner::instance = NULL;

BuildPlanner::BuildPlanner()
{
	BuildOrderFileReader br = BuildOrderFileReader();
	buildOrder = br.readBuildOrder();
	lastCallFrame = Broodwar->getFrameCount();
}

BuildPlanner::~BuildPlanner()
{
	delete instance;
}

BuildPlanner* BuildPlanner::getInstance()
{
	if (instance == NULL)
	{
		instance = new BuildPlanner();
	}
	return instance;
}

void BuildPlanner::buildingDestroyed(Unit* building)
{
	if (building->getType().getID() == UnitTypes::Protoss_Pylon.getID())
	{
		return;
	}
	if (building->getType().getID() == UnitTypes::Terran_Supply_Depot.getID())
	{
		return;
	}
	if (building->getType().isAddon())
	{
		return;
	}
	buildOrder.insert(buildOrder.begin(), building->getType());
}

void BuildPlanner::computeActions()
{
	//Dont call too often
	int cFrame = Broodwar->getFrameCount();
	if (cFrame - lastCallFrame < 40)
	{
		return;
	}
	lastCallFrame = cFrame;

	if (AgentManager::getInstance()->getNoWorkers() == 0)
	{
		//No workers so cant do anything
		return;
	}

	//Check if we have possible "locked" items in the buildqueue
	for (int i = 0; i < (int)buildQueue.size(); i++)
	{
		int elapsed = cFrame - buildQueue.at(i).assignedFrame;
		if (elapsed >= 2000)
		{
			Broodwar->printf("Failed to build %s in time, resetting order", buildQueue.at(i).toBuild.getName().c_str());

			//Reset the build request
			WorkerAgent* worker = (WorkerAgent*)AgentManager::getInstance()->getAgent(buildQueue.at(i).assignedWorkerId);
			if (worker != NULL)
			{
				worker->reset();
			}
			buildOrder.insert(buildOrder.begin(), buildQueue.at(i).toBuild);
			ResourceManager::getInstance()->unlockResources(buildQueue.at(i).toBuild);
			buildQueue.erase(buildQueue.begin() + i);
			return;
		}
	}


	//Check if we need more supply buildings
	if (isTerran() || isProtoss())
	{
		if (shallBuildSupply())
		{
			buildOrder.insert(buildOrder.begin(), Broodwar->self()->getRace().getSupplyProvider());
		}
	}

	//Check if we can build next building in the buildorder
	if ((int)buildOrder.size() > 0)
	{
		executeOrder(buildOrder.at(0));
	}

	if (!hasResourcesLeft() || ResourceManager::getInstance()->hasResources(2000, 0, false))
	{
		expand(Broodwar->self()->getRace().getCenter());
	}

}

bool BuildPlanner::hasResourcesLeft()
{
	int totalMineralsLeft = 0;

	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->getUnitType().isResourceDepot())
		{
			totalMineralsLeft += mineralsNearby(agent->getUnit()->getTilePosition());
		}
	}

	//Broodwar->printf("Minerals left: %d", totalMineralsLeft);

	if (totalMineralsLeft <= 8000) //TODO: Diminishing limits for expanding e.g. 10k - eXpands * Y
	{
		return false;
	}
	return true;
}

int BuildPlanner::mineralsNearby(TilePosition center)
{
	int mineralCnt = 0;

	for(set<Unit*>::iterator m = Broodwar->getMinerals().begin(); m != Broodwar->getMinerals().end(); m++)
	{
		if ((*m)->exists())
		{
			double dist = center.getDistance((*m)->getTilePosition());
			if (dist <= 10)
			{
				mineralCnt += (*m)->getResources();			
			}
		}
	}

	return mineralCnt;
}

bool BuildPlanner::shallBuildSupply()
{
	UnitType supply = Broodwar->self()->getRace().getSupplyProvider();

	//1. If command center is next in queue, dont build pylon
	/*if (buildOrder.size() > 0)
	{
		if (buildOrder.at(0).isResourceDepot())
		{
			return false;
		}
	}*/

	//2. Check if any building is unpowered (Protoss only)
	if (isProtoss())
	{
		if (buildOrder.size() > 0)
		{
			if (buildOrder.at(0).getID() != UnitTypes::Protoss_Pylon.getID())
			{
				vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
				for (int i = 0; i < (int)agents.size(); i++)
				{
					BaseAgent* agent = agents.at(i);
					if (agent->isAlive())
				{
						Unit* cUnit = agent->getUnit();
						if (cUnit->isUnpowered())
				{
							return true;
						}
					}
				}
			}
		}
	}

	//3. Check if we need supplies
	int supplyTotal = Broodwar->self()->supplyTotal() / 2;
	int supplyUsed = Broodwar->self()->supplyUsed() / 2;
	if (supplyTotal - supplyUsed > 8)
	{
		return false;
	}

	if (supplyTotal >= 200)
	{
		//Reached max supply
		return false;
	}

	//4. Check if there is a supply already in the list
	if (nextIsOfType(supply))
	{
		return false;
	}

	//5. Check if we are already building a supply
	if (supplyBeingBuilt())
	{
		return false;
	}

	//Broodwar->printf("Supplies: %d/%d. Adding supply to buildorder", supplyUsed, supplyTotal);

	return true;
}

bool BuildPlanner::supplyBeingBuilt()
{
	//Zerg
	if (isZerg())
	{
		if (noInProduction(UnitTypes::Zerg_Overlord) > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	//Terran and Protoss
	UnitType supply = Broodwar->self()->getRace().getSupplyProvider();

	//1. Check if we are already building a supply
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive())
		{
			if (agent->getUnitType().getID() == supply.getID())
			{
				if (agent->getUnit()->isBeingConstructed())
				{
					//Found one that is being constructed
					return true;
				}
			}
		}
	}

	//2. Check if we have a supply in build queue
	for (int i = 0; i < (int)buildQueue.size(); i++)
	{
		if (buildQueue.at(i).toBuild.getID() == supply.getID())
		{
			return true;
		}
	}

	return false;
}

void BuildPlanner::lock(int buildOrderIndex, int unitId)
{
	UnitType type = buildOrder.at(buildOrderIndex);
	buildOrder.erase(buildOrder.begin() + buildOrderIndex);

	BuildQueueItem item;
	item.toBuild = type;
	item.assignedWorkerId = unitId;
	item.assignedFrame = Broodwar->getFrameCount();

	buildQueue.push_back(item);
}

void BuildPlanner::remove(UnitType type)
{
	for (int i = 0; i < (int)buildOrder.size(); i++)
	{
		if (buildOrder.at(i).getID() == type.getID())
		{
			buildOrder.erase(buildOrder.begin() + i);
			return;
		}
	}
}

void BuildPlanner::unlock(UnitType type)
{
	for (int i = 0; i < (int)buildQueue.size(); i++)
	{
		if (buildQueue.at(i).toBuild.getID() == type.getID())
		{
			buildQueue.erase(buildQueue.begin() + i);
			return;
		}
	}
}

void BuildPlanner::handleWorkerDestroyed(UnitType type, int workerID)
{
	for (int i = 0; i < (int)buildQueue.size(); i++)
	{
		if (buildQueue.at(i).assignedWorkerId == workerID)
		{
			buildQueue.erase(buildQueue.begin() + i);
			buildOrder.insert(buildOrder.begin(), type);
			ResourceManager::getInstance()->unlockResources(type);
		}
	}
}

bool BuildPlanner::executeMorph(UnitType target, UnitType evolved)
{
	BaseAgent* agent = AgentManager::getInstance()->getClosestAgent(Broodwar->self()->getStartLocation(), target);
	if (agent != NULL)
	{
		StructureAgent* sAgent = (StructureAgent*)agent;
		if (sAgent->canMorphInto(evolved))
		{
			sAgent->getUnit()->morph(evolved);
			lock(0, sAgent->getUnitID());
			return true;
		}
	}
	else
	{
		//No building available that can do this morph.
		remove(evolved);
	}
	return false;
}

bool BuildPlanner::executeOrder(UnitType type)
{
	//Max 3 concurrent buildings allowed at the same time
	if ((int)buildQueue.size() >= 3)
	{
		return false;
	}

	////Hold if we are to build a new base
	//if ((int)buildQueue.size() > 0)
	//{
	//	if (buildQueue.at(0).toBuild.isResourceDepot())
	//	{
	//		return false;
	//	}
	//	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	//	for (int i = 0; i < (int)agents.size(); i++)
	//	{
	//		if (agents.at(i)->getUnitType().isResourceDepot() && agents.at(i)->getUnit()->isBeingConstructed())
	//		{
	//			return false; // TODO: Check if necessary
	//		}
	//	}
	//}

	if (type.isResourceDepot())
	{
		TilePosition pos = CoverMap::getInstance()->findExpansionSite();
		if (pos.x() == -1)
		{
			Broodwar->printf("Removing expand as no expansion site could be found");
			//No expansion site found.
			if ((int)buildOrder.size() > 0) buildOrder.erase(buildOrder.begin());
			return true;
		}
	}

	if (type.isRefinery())
	{
		TilePosition rSpot = CoverMap::getInstance()->searchRefinerySpot();
		if (rSpot.x() < 0)
		{
			//No buildspot found
			if ((int)buildOrder.size() > 0) buildOrder.erase(buildOrder.begin());
			return true;
		}
	}

	if (isZerg())
	{
		pair<UnitType, int> builder = type.whatBuilds();
		if (builder.first.getID() != UnitTypes::Zerg_Drone)
		{
			//Needs to be morphed
			return executeMorph(builder.first, type);
		}
	}

	//Check if we have resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}


	// TODO: Check if we can improve finding the best worker
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent != NULL && agent->isAlive())
		{
			if (agent->canBuild(type))
			{
				if (agent->assignToBuild(type))
				{
					lock(0, agent->getUnitID());
					return true;
				}
				else
				{
					//Unable to find a buildspot. Dont bother checking for all
					//other workers
					handleNoBuildspotFound(type);

					Broodwar->printf("No build spot found for %s", type.getName().c_str());

					return false;
				}
			}
		}
	}
	return false;
}

bool BuildPlanner::isTerran()
{
	if (Broodwar->self()->getRace().getID() == Races::Terran.getID())
	{
		return true;
	}
	return false;
}

bool BuildPlanner::isProtoss()
{
	if (Broodwar->self()->getRace().getID() == Races::Protoss.getID())
	{
		return true;
	}
	return false;
}

bool BuildPlanner::isZerg()
{
	if (Broodwar->self()->getRace().getID() == Races::Zerg.getID())
	{
		return true;
	}
	return false;
}

void BuildPlanner::addRefinery()
{
	UnitType refinery = Broodwar->self()->getRace().getRefinery();

	if (!this->nextIsOfType(refinery))
	{
		buildOrder.insert(buildOrder.begin(), refinery);
	}
}

void BuildPlanner::commandCenterBuilt()
{
	lastCommandCenter = Broodwar->getFrameCount();
}

string BuildPlanner::format(UnitType type)
{
	string name = type.getName();
	int i = name.find(" ");
	string fname = name.substr(i + 1, name.length());
	return fname;
}

void BuildPlanner::printInfo()
{
	int max = 4;
	if ((int)buildOrder.size() < 4)
	{
		max = (int)buildOrder.size();
	}

	int line = 1;
	Broodwar->drawTextScreen(5,0,"Buildorder:");
	for (int i = 0; i < max; i++)
	{
		Broodwar->drawTextScreen(5,16*line, format(buildOrder.at(i)).c_str());
		line++;
	}

	int qmax = 4;
	if ((int)buildQueue.size() < 4)
	{
		qmax = (int)buildQueue.size();
	}

	line = 1;
	Broodwar->drawTextScreen(150,0,"Buildqueue:");
	for (int i = 0; i < qmax; i++)
	{
		Broodwar->drawTextScreen(150,16*line, format(buildQueue.at(i).toBuild).c_str());
		line++;
	}
}

void BuildPlanner::handleNoBuildspotFound(UnitType toBuild)
{
	bool removeOrder = false;
	if (toBuild.getID() == UnitTypes::Protoss_Photon_Cannon) removeOrder = true;
	if (toBuild.getID() == UnitTypes::Terran_Missile_Turret) removeOrder = true;
	if (toBuild.isAddon()) removeOrder = true;
	if (toBuild.getID() == UnitTypes::Zerg_Spore_Colony) removeOrder = true;
	if (toBuild.getID() == UnitTypes::Zerg_Sunken_Colony) removeOrder = true;
	if (toBuild.isResourceDepot()) removeOrder = true;
	if (toBuild.isRefinery()) removeOrder = true;

	if (removeOrder)
	{
		remove(toBuild);
	}
	else
	{
		if (isProtoss() && !supplyBeingBuilt())
		{
			//Insert a pylon to increase PSI coverage
			if (!nextIsOfType(UnitTypes::Protoss_Pylon))
			{
				buildOrder.insert(buildOrder.begin(), UnitTypes::Protoss_Pylon);
			}
		}
	}
}

bool BuildPlanner::nextIsOfType(UnitType type)
{
	if ((int)buildOrder.size() == 0)
	{
		return false;
	}
	else
	{
		if (buildOrder.at(0).getID() == type.getID())
		{
			return true;
		}
	}
	return false;
}

bool BuildPlanner::containsType(UnitType type)
{
	for (int i = 0; i < (int)buildOrder.size(); i++)
	{
		if (buildOrder.at(i).getID() == type.getID())
		{
			return true;
		}
	}
	return false;
}

bool BuildPlanner::coveredByDetector(TilePosition pos)
{
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive())
		{
			UnitType type = agent->getUnitType();
			if (type.isDetector() && type.isBuilding())
			{
				double range = type.sightRange() * 1.5;
				double dist = agent->getUnit()->getPosition().getDistance(Position(pos));
				if (dist <= range)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void BuildPlanner::addBuilding(UnitType type)
{
	buildOrder.push_back(type);
}

void BuildPlanner::addBuildingFirst(UnitType type)
{
	buildOrder.insert(buildOrder.begin(), type);
}

void BuildPlanner::expand(UnitType commandCenterUnit)
{
	if (containsType(commandCenterUnit))
	{
		return;
	}

	TilePosition pos = CoverMap::getInstance()->findExpansionSite();
	if (pos.x() == -1)
	{
		//No expansion site found.
		return;
	}

	buildOrder.insert(buildOrder.begin(), commandCenterUnit);
}

int BuildPlanner::noInProduction(UnitType type)
{
	int no = 0;
	
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive())
		{
			if (agent->getUnitType().canProduce() && !agent->getUnit()->isBeingConstructed())
			{
				list<UnitType> queue = agent->getUnit()->getTrainingQueue();
				for (list<UnitType>::const_iterator i=queue.begin(); i != queue.end(); i++)
				{
					if ((*i).getID() == type.getID())
				{
						no++;
					}
				}
			}
		}
	}

	if (isZerg())
	{
		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
		{
			if ((*i)->exists())
			{
				if ((*i)->getType().getID() == UnitTypes::Zerg_Egg.getID())
				{
					if ((*i)->getBuildType().getID() == type.getID())
					{
						no++;
						if (type.isTwoUnitsInOneEgg()) no++;
					}
				}
			}
		}
	}

	return no;
}
