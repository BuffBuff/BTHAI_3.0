#include "AIloop.h"
#include "CoverMap.h"
#include "Profiler.h"
#include "ResourceManager.h"

AIloop::AIloop()
{
	
}

AIloop::~AIloop()
{
	
}

void AIloop::setDebugMode(int mode)
{
	debug_mode = mode;
}

void AIloop::computeActions()
{
	//First, do some checks to see if it is time to resign
	if (AgentManager::getInstance()->noMiningWorkers() == 0 && Broodwar->self()->minerals() <= 50)
	{
		Broodwar->printf("No workers left. Bailing out.");
		Broodwar->leaveGame();
		return;
	}
	if (AgentManager::getInstance()->countNoBases() == 0 && Broodwar->getFrameCount() > 500)
	{
		Broodwar->printf("No bases left. Bailing out.");
		Broodwar->leaveGame();
		return;
	}

	AgentManager::getInstance()->computeActions();
	BuildPlanner::getInstance()->computeActions();
	Commander::getInstance()->computeActions();
	ExplorationManager::getInstance()->computeActions();
}

void AIloop::addUnit(Unit* unit)
{
	AgentManager::getInstance()->addAgent(unit);

	//Remove from buildorder if this is a building
	if (unit->getType().isBuilding())
	{
		BuildPlanner::getInstance()->unlock(unit->getType());
	}
}

void AIloop::morphUnit(Unit* unit)
{
	AgentManager::getInstance()->morphDrone(unit);
	BuildPlanner::getInstance()->unlock(unit->getType());
}

void AIloop::unitDestroyed(Unit* unit)
{
	if (unit->getPlayer()->getID() == Broodwar->self()->getID())
	{
		AgentManager::getInstance()->removeAgent(unit);
		if (unit->getType().isBuilding())
		{
			BuildPlanner::getInstance()->buildingDestroyed(unit);
		}

		//Assist workers under attack
		if (unit->getType().isWorker())
		{
			Commander::getInstance()->assistWorker(AgentManager::getInstance()->getAgent(unit->getID()));
		}

		//Update dead score
		if (unit->getType().canMove())
		{
			Commander::getInstance()->ownDeadScore += unit->getType().destroyScore();
		}

		AgentManager::getInstance()->cleanup();
	}
	if (unit->getPlayer()->getID() != Broodwar->self()->getID() && !unit->getPlayer()->isNeutral())
	{
		ExplorationManager::getInstance()->unitDestroyed(unit);
		Commander::getInstance()->enemyDeadScore += unit->getType().destroyScore();
	}
}

void AIloop::show_debug()
{
	if (debug_mode > 0)
	{
		vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
		for (int i = 0; i < (int)agents.size(); i++)
		{
			if (agents.at(i)->isBuilding()) agents.at(i)->debug_showGoal();
			if (!agents.at(i)->isBuilding() && debug_mode >= 2) agents.at(i)->debug_showGoal();
		}

		BuildPlanner::getInstance()->printInfo();
		ExplorationManager::getInstance()->printInfo();
		Commander::getInstance()->printInfo();
		
		if (debug_mode >= 3) CoverMap::getInstance()->debug();
		if (debug_mode >= 2) ResourceManager::getInstance()->printInfo();

		Commander::getInstance()->debug_showGoal();

		if (debug_mode >= 1)
		{
			drawTerrainData();
		}
	}
}

void AIloop::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
	{
		TilePosition p=(*i)->getTilePosition();
		Position c=(*i)->getPosition();

		//Draw a progress bar at each resource
		for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getStaticMinerals().begin();j!=(*i)->getStaticMinerals().end();j++)
		{
			if ((*j)->getResources() > 0)
			{
				
				int total = (*j)->getInitialResources();
				int done = (*j)->getResources();

				int w = 60;
				int h = 64;

				//Start 
				Position s = Position((*j)->getPosition().x() - w/2 + 2, (*j)->getPosition().y() - 4);
				//End
				Position e = Position(s.x() + w, s.y() + 8);
				//Progress
				int prg = (int)((double)done / (double)total * w);
				Position p = Position(s.x() + prg, s.y() +  8);

				Broodwar->drawBox(CoordinateType::Map,s.x(),s.y(),e.x(),e.y(),Colors::Orange,false);
				Broodwar->drawBox(CoordinateType::Map,s.x(),s.y(),p.x(),p.y(),Colors::Orange,true);
			}
		}
	}

	if (debug_mode >= 2)
	{
		//we will iterate through all the regions and draw the polygon outline of it in white.
		for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
		{
			BWTA::Polygon p=(*r)->getPolygon();
			for(int j=0;j<(int)p.size();j++)
			{
				Position point1=p[j];
				Position point2=p[(j+1) % p.size()];
				Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Brown);
			}
		}

		//we will visualize the chokepoints with yellow lines
		for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
		{
			for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
			{
				Position point1=(*c)->getSides().first;
				Position point2=(*c)->getSides().second;
				Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Yellow);
			}
		}
	}

	//locate zerg eggs and draw progress bars
	if (BuildPlanner::isZerg())
	{
		for(std::set<Unit*>::const_iterator u = Broodwar->self()->getUnits().begin(); u != Broodwar->self()->getUnits().end(); u++)
		{
			if ((*u)->getType().getID() == UnitTypes::Zerg_Egg.getID() || (*u)->getType().getID() == UnitTypes::Zerg_Lurker_Egg.getID() || (*u)->getType().getID() == UnitTypes::Zerg_Cocoon.getID())
			{
				int total = (*u)->getBuildType().buildTime();
				int done = total - (*u)->getRemainingBuildTime();
				
				int w = (*u)->getType().tileWidth() * 32;
				int h = (*u)->getType().tileHeight() * 32;

				//Start 
				Position s = Position((*u)->getPosition().x() - w/2, (*u)->getPosition().y() - 4);
				//End
				Position e = Position(s.x() + w, s.y() + 8);
				//Progress
				int prg = (int)((double)done / (double)total * w);
				Position p = Position(s.x() + prg, s.y() +  8);

				Broodwar->drawBox(CoordinateType::Map,s.x(),s.y(),e.x(),e.y(),Colors::Blue,false);
				Broodwar->drawBox(CoordinateType::Map,s.x(),s.y(),p.x(),p.y(),Colors::Blue,true);
			}
		}
	}
}
