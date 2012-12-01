#include "UpgradesPlanner.h"
#include "UpgradesFileReader.h"
#include "BuildPlanner.h"
#include "AgentManager.h"
#include "ResourceManager.h"

UpgradesPlanner* UpgradesPlanner::instance = NULL;

UpgradesPlanner::UpgradesPlanner()
{
	UpgradesFileReader ufr = UpgradesFileReader();
	ufr.readUpgrades();

	upgradesP1 = ufr.getUpgradesP1();
	upgradesP2 = ufr.getUpgradesP2();
	upgradesP3 = ufr.getUpgradesP3();
	techsP1 = ufr.getTechsP1();
	techsP2 = ufr.getTechsP2();
	techsP3 = ufr.getTechsP3();
}

UpgradesPlanner::~UpgradesPlanner()
{
	delete instance;
}

UpgradesPlanner* UpgradesPlanner::getInstance()
{
	if (instance == NULL)
	{
		instance = new UpgradesPlanner();
	}
	return instance;
}

bool UpgradesPlanner::checkUpgrade(BaseAgent* agent)
{
	if (agent->isAlive() && agent->getUnit()->isIdle())
	{
		Unit* unit = agent->getUnit();

		//Check techsP1
		for (int i = 0; i < (int)techsP1.size(); i++)
		{
			TechType type = techsP1.at(i);
			if (Broodwar->self()->hasResearched(type))
			{
				techsP1.erase(techsP1.begin() + i);
				return true;
			}
			if (canResearch(type, unit))
			{
				unit->research(type);
				return true;
			}
		}
		//Check techsP2
		if ((int)techsP1.size() == 0)
		{
			for (int i = 0; i < (int)techsP2.size(); i++)
			{
				TechType type = techsP2.at(i);
				if (Broodwar->self()->hasResearched(type))
				{
					techsP2.erase(techsP2.begin() + i);
					return true;
				}
				if (canResearch(type, unit))
				{
					unit->research(type);
					return true;
				}
			}
		}
		//Check techsP3
		if ((int)techsP1.size() == 0 && (int)techsP2.size() == 0)
		{
			for (int i = 0; i < (int)techsP3.size(); i++)
			{
				TechType type = techsP3.at(i);
				if (Broodwar->self()->hasResearched(type))
				{
					techsP3.erase(techsP3.begin() + i);
					return true;
				}
				if (canResearch(type, unit))
				{
					unit->research(type);
					return true;
				}
			}
		}

		//Check upgradesP1
		for (int i = 0; i < (int)upgradesP1.size(); i++)
		{
			UpgradeType type = upgradesP1.at(i);
			if (canUpgrade(type, unit))
			{
				if (unit->upgrade(type))
				{
					upgradesP1.erase(upgradesP1.begin() + i);
					return true;
				}
			}
		}
		//Check upgradesP2
		if ((int)upgradesP1.size() == 0)
		{
			for (int i = 0; i < (int)upgradesP2.size(); i++)
			{
				UpgradeType type = upgradesP2.at(i);
				if (canUpgrade(type, unit))
				{
					if (unit->upgrade(type))
				{
						upgradesP2.erase(upgradesP2.begin() + i);
						return true;
					}
				}
			}
		}
		//Check upgradesP3
		if ((int)upgradesP1.size() == 0 && (int)upgradesP2.size() == 0)
		{
			for (int i = 0; i < (int)upgradesP3.size(); i++)
			{
				UpgradeType type = upgradesP3.at(i);
				if (canUpgrade(type, unit))
				{
					if (unit->upgrade(type))
				{
						upgradesP3.erase(upgradesP3.begin() + i);
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool UpgradesPlanner::canUpgrade(UpgradeType type, Unit* unit)
{
	//1. Check if unit is idle
	if (!unit->isIdle())
	{
		return false;
	}

	//2. Check if unit can do this upgrade
	if (!Broodwar->canUpgrade(unit, type))
	{
		return false;
	}

	//3. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}
	
	//4. Check if unit is being constructed
	if (unit->isBeingConstructed())
	{
		return false;
	}

	//5. Check if some other building is already doing this upgrade
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->getUnit()->getUpgrade().getID() == type.getID())
		{
			return false;
		}
	}

	//6. Check if we are currently upgrading it
	if (Broodwar->self()->isUpgrading(type))
	{
		return false;
	}

	//All clear. Can do the upgrade.
	return true;
}

bool UpgradesPlanner::canResearch(TechType type, Unit* unit)
{
	//1. Check if unit can do this upgrade
	if (!Broodwar->canResearch(unit, type))
	{
		return false;
	}
	
	//2. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}
	
	//3. Check if unit is idle
	if (!unit->isIdle())
	{
		return false;
	}

	//4. Check if unit is being constructed
	if (unit->isBeingConstructed())
	{
		return false;
	}

	//5. Check if some other building is already doing this upgrade
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->getUnit()->getTech().getID() == type.getID())
		{
			return false;
		}
	}

	//6. Check if we are currently researching it
	if (Broodwar->self()->isResearching(type))
	{
		return false;
	}

	//All clear. Can do the research.
	return true;
}
