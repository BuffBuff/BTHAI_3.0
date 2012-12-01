#include "ResourceManager.h"
#include "BuildPlanner.h"
#include "AgentManager.h"
#include "Commander.h"
#include "Squad.h"

ResourceManager* ResourceManager::instance = NULL;

ResourceManager::ResourceManager()
{
	locks.push_back(ResourceLock(Broodwar->self()->getRace().getCenter()));
}

ResourceManager::~ResourceManager()
{
	delete instance;
}

ResourceManager* ResourceManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new ResourceManager();
	}
	return instance;
}

bool ResourceManager::hasProductionBuilding()
{
	if (BuildPlanner::isTerran())
	{
		if (AgentManager::getInstance()->countNoUnits(UnitTypes::Terran_Barracks) > 0) return true;
	}
	if (BuildPlanner::isProtoss())
	{
		if (AgentManager::getInstance()->countNoUnits(UnitTypes::Protoss_Gateway) > 0) return true;
	}
	if (BuildPlanner::isZerg())
	{
		if (AgentManager::getInstance()->countNoUnits(UnitTypes::Zerg_Hydralisk_Den) > 0) return true;
	}
	return false;
}

bool ResourceManager::needWorker()
{
	int workersPerBase = 18;
	if (BuildPlanner::isZerg())
	{
		workersPerBase = 10;
	}

	int noBases = AgentManager::getInstance()->countNoBases();
	int noRefinery = AgentManager::getInstance()->countNoUnits(Broodwar->self()->getRace().getRefinery());
	int idealNoWorkers = noBases * workersPerBase + noRefinery * 3;
	if (idealNoWorkers > 60) idealNoWorkers = 60;

	int noWorkers = AgentManager::getInstance()->getNoWorkers();
	if (noWorkers < idealNoWorkers)
	{
		//Check if we have enough resources
		int nMinerals = Broodwar->self()->getRace().getWorker().mineralPrice();
		if (hasProductionBuilding())
		{
			nMinerals += 150;
		}
		return hasResources(nMinerals, 0);
	}
	return false;
}

bool ResourceManager::hasResources(UnitType type)
{
	int nMinerals = type.mineralPrice();
	int nGas = type.gasPrice();

	if (type.isBuilding())
	{
		if (hasProductionBuilding())
		{
			nMinerals += 150;
			if (nGas > 0)
			{
				nGas += 100;
			}
		}
	}

	return hasResources(nMinerals, nGas);
}

bool ResourceManager::hasResources(UpgradeType type)
{
	int nMinerals = type.mineralPrice();
	int nGas = type.gasPrice();

	return hasResources(nMinerals, nGas);
}

bool ResourceManager::hasResources(TechType type)
{
	int nMinerals = type.mineralPrice();
	int nGas = type.gasPrice();

	return hasResources(nMinerals, nGas);
}

bool ResourceManager::hasResources(int neededMinerals, int neededGas)
{
	if (Broodwar->self()->minerals() - calcLockedMinerals() >= neededMinerals)
	{
		if (Broodwar->self()->gas() - calcLockedGas() >= neededGas)
		{
			return true;
		}
	}
	return false;
}

void ResourceManager::lockResources(UnitType type)
{
	locks.push_back(type);
}

void ResourceManager::unlockResources(UnitType type)
{
	for (int i = 0; i < (int)locks.size(); i++)
	{
		if (locks.at(i).unit.getID() == type.getID())
		{
			locks.erase(locks.begin() + i);
			return;
		}
	}
}

int ResourceManager::calcLockedMinerals()
{
	int nMinerals = 0;

	for (int i = 0; i < (int)locks.size(); i++)
	{
		nMinerals += locks.at(i).mineralCost;
	}

	return nMinerals;
}

int ResourceManager::calcLockedGas()
{
	int nGas = 0;

	for (int i = 0; i < (int)locks.size(); i++)
	{
		nGas += locks.at(i).gasCost;
	}

	return nGas;
}

void ResourceManager::printInfo()
{
	Broodwar->drawTextScreen(5,96, "Locked minerals: %d", calcLockedMinerals());
	Broodwar->drawTextScreen(5,112, "Locked gas: %d", calcLockedGas());
}
