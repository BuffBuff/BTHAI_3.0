#include "HatcheryAgent.h"
#include "AgentManager.h"
#include "WorkerAgent.h"
#include "PFManager.h"
#include "BuildPlanner.h"
#include "Commander.h"
#include "UpgradesPlanner.h"
#include "ResourceManager.h"

HatcheryAgent::HatcheryAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	//Broodwar->printf("New base created at (%d,%d)", unit->getTilePosition().x(), unit->getTilePosition().y());

	hasSentWorkers = false;
	if (AgentManager::getInstance()->countNoBases() == 0)
	{
		//We dont do this for the first Nexus.
		hasSentWorkers = true;
	}
	if (mUnit->getType().getID() == UnitTypes::Zerg_Lair.getID())
	{
		//Upgrade. Dont send workers.
		hasSentWorkers = true;
	}
	if (mUnit->getType().getID() == UnitTypes::Zerg_Hive.getID())
	{
		//Upgrade. Dont send workers.
		hasSentWorkers = true;
	}
	
	agentType = "HatcheryAgent";
	BuildPlanner::getInstance()->commandCenterBuilt();
}

void HatcheryAgent::computeActions()
{
	if (!hasSentWorkers)
	{
		if (!unit->isBeingConstructed())
		{
			sendWorkers();
			hasSentWorkers = true;
			BuildPlanner::getInstance()->addRefinery();

			/*if (AgentManager::getInstance()->countNoUnits(UnitTypes::Zerg_Spawning_Pool) > 0)
			{
				BuildPlanner::getInstance()->addBuildingFirst(UnitTypes::Zerg_Creep_Colony);
				BuildPlanner::getInstance()->addBuildingFirst(UnitTypes::Zerg_Sunken_Colony);
			}*/
		}
	}

	if (!unit->isIdle())
	{
		//Already doing something
		return;
	}
	
	//Build Overlords for supply
	if (BuildPlanner::getInstance()->shallBuildSupply())
	{
		if (canBuild(UnitTypes::Zerg_Overlord))
		{
			unit->train(UnitTypes::Zerg_Overlord);
			return;
		}
	}
	//Build units
	if (checkBuildUnit(UnitTypes::Zerg_Queen)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Mutalisk)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Hydralisk)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Zergling)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Defiler)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Ultralisk)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Scourge)) return;

	//Create workers
	if (ResourceManager::getInstance()->needWorker())
	{
		UnitType worker = Broodwar->self()->getRace().getWorker();
		if (canBuild(worker))
		{
			unit->train(worker);
		}
	}

	//Check for base upgrades
	if (isOfType(UnitTypes::Zerg_Hatchery))
	{
		if (AgentManager::getInstance()->countNoUnits(UnitTypes::Zerg_Lair) < 1)
		{
			if (Broodwar->canMake(unit, UnitTypes::Zerg_Lair))
			{
				if (ResourceManager::getInstance()->hasResources(UnitTypes::Zerg_Lair))
				{
					ResourceManager::getInstance()->lockResources(UnitTypes::Zerg_Lair);
					unit->morph(UnitTypes::Zerg_Lair);
					return;
				}
			}
		}
	}
	if (isOfType(UnitTypes::Zerg_Lair))
	{
		if (AgentManager::getInstance()->countNoUnits(UnitTypes::Zerg_Hive) < 1)
		{
			if (Broodwar->canMake(unit, UnitTypes::Zerg_Hive))
			{
				if (ResourceManager::getInstance()->hasResources(UnitTypes::Zerg_Hive))
				{
					ResourceManager::getInstance()->lockResources(UnitTypes::Zerg_Hive);
					unit->morph(UnitTypes::Zerg_Hive);
					return;
				}
			}
		}
	}
	

	//Check for upgrades
	UpgradesPlanner::getInstance()->checkUpgrade(this);
}

bool HatcheryAgent::checkBuildUnit(UnitType type)
{
	if (canEvolveUnit(type))
	{
		unit->train(type);
		return true;
	}
	return false;
}

