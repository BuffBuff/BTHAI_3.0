#include "AgentFactory.h"
#include "WorkerAgent.h"
#include "StructureAgent.h"
#include "UnitAgent.h"
#include "Commander.h"
#include "TransportAgent.h"

/** Terran agents */
#include "CommandCenterAgent.h"
#include "SiegeTankAgent.h"
#include "MarineAgent.h"
#include "MedicAgent.h"
#include "VultureAgent.h"
#include "FirebatAgent.h"
#include "RefineryAgent.h"
#include "ScienceVesselAgent.h"
#include "BattlecruiserAgent.h"
#include "WraithAgent.h"
#include "GhostAgent.h"
#include "GoliathAgent.h"
#include "ValkyrieAgent.h"

/** Protoss agents */
#include "NexusAgent.h"
#include "ZealotAgent.h"
#include "DragoonAgent.h"
#include "ReaverAgent.h"
#include "ObserverAgent.h"
#include "DarkTemplarAgent.h"
#include "ScoutAgent.h"
#include "CorsairAgent.h"
#include "CarrierAgent.h"
#include "HighTemplarAgent.h"

/** Zerg agents */
#include "HatcheryAgent.h"
#include "OverlordAgent.h"
#include "ZerglingAgent.h"
#include "HydraliskAgent.h"
#include "LurkerAgent.h"
#include "MutaliskAgent.h"
#include "QueenAgent.h"
#include "UltraliskAgent.h"
#include "DevourerAgent.h"
#include "GuardianAgent.h"
#include "DefilerAgent.h"
#include "ScourgeAgent.h"
#include "InfestedTerranAgent.h"

bool AgentFactory::instanceFlag = false;
AgentFactory* AgentFactory::instance = NULL;

AgentFactory::AgentFactory()
{
	
}

AgentFactory::~AgentFactory()
{
	instanceFlag = false;
	delete instance;
}

AgentFactory* AgentFactory::getInstance()
{
	if (!instanceFlag)
	{
		instance = new AgentFactory();
		instanceFlag = true;
	}
	return instance;
}

BaseAgent* AgentFactory::createAgent(Unit* unit)
{
	if (Broodwar->self()->getRace().getID() == Races::Terran.getID())
	{
		return createTerranAgent(unit);
	}
	if (Broodwar->self()->getRace().getID() == Races::Protoss.getID())
	{
		return createProtossAgent(unit);
	}
	if (Broodwar->self()->getRace().getID() == Races::Zerg.getID())
	{
		return createZergAgent(unit);
	}

	//Default agents
	if (unit->getType().isWorker())
	{
		return new WorkerAgent(unit);
	}
	else if (unit->getType().isBuilding())
	{
		return new StructureAgent(unit);
	}
	else
	{
		return new UnitAgent(unit);
	}
}

BaseAgent* AgentFactory::createZergAgent(Unit* unit)
{
	UnitType type = unit->getType();
	
	if (type.isWorker())
	{
		return new WorkerAgent(unit);
	}
	else if (type.isBuilding())
	{
		//Add agents for special buildings here
		if (type.getID() == UnitTypes::Zerg_Hatchery.getID())
		{
			return new HatcheryAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Lair.getID())
		{
			return new HatcheryAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Hive.getID())
		{
			return new HatcheryAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Extractor.getID())
		{
			return new RefineryAgent(unit);
		}
		else
		{
			//Default structure agent
			return new StructureAgent(unit);
		}
	}
	else
	{
#if DISABLE_UNIT_AI == 0
		if (type.getID() == UnitTypes::Zerg_Overlord.getID())
		{
			return new OverlordAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Zergling.getID())
		{
			return new ZerglingAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Hydralisk.getID())
		{
			return new HydraliskAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Mutalisk.getID())
		{
			return new MutaliskAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Lurker.getID())
		{
			return new LurkerAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Queen.getID())
		{
			return new QueenAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Ultralisk.getID())
		{
			return new UltraliskAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Guardian.getID())
		{
			return new GuardianAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Devourer.getID())
		{
			return new DevourerAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Defiler.getID())
		{
			return new DefilerAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Scourge.getID())
		{
			return new ScourgeAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Infested_Terran.getID())
		{
			return new InfestedTerranAgent(unit);
		}
		else
		{
			//Default unit agent
			return new UnitAgent(unit);
		}
#else
		return new UnitAgent(unit);
#endif
	}
	return NULL;
}

BaseAgent* AgentFactory::createTerranAgent(Unit* unit)
{
	if (unit->getType().isWorker())
	{
		return new WorkerAgent(unit);
	}
	else if (unit->getType().isBuilding())
	{
		//Add agents for special buildings here
		if (isOfType(unit, UnitTypes::Terran_Command_Center))
		{
			return new CommandCenterAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Refinery))
		{
			return new RefineryAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Bunker))
		{
			Commander::getInstance()->addBunkerSquad();
			return new StructureAgent(unit);
		}
		else
		{
			//Default structure agent
			return new StructureAgent(unit);
		}
	}
	else
	{
#if DISABLE_UNIT_AI == 0
		if (isOfType(unit, UnitTypes::Terran_Siege_Tank_Tank_Mode))
		{
			return new SiegeTankAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Siege_Tank_Siege_Mode))
		{
			return new SiegeTankAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Marine))
		{
			return new MarineAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Firebat))
		{
			return new FirebatAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Medic))
		{
			return new MedicAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Vulture))
		{
			return new VultureAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Science_Vessel))
		{
			return new ScienceVesselAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Battlecruiser))
		{
			return new BattlecruiserAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Wraith))
		{
			return new WraithAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Ghost))
		{
			return new GhostAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Goliath))
		{
			return new GoliathAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Valkyrie))
		{
			return new ValkyrieAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Dropship))
		{
			return new TransportAgent(unit);
		}
		else
		{
			//Default unit agent
			return new UnitAgent(unit);
		}
#else
		return new UnitAgent(unit);
#endif
	}
	return NULL;
}

BaseAgent* AgentFactory::createProtossAgent(Unit* unit)
{
	if (unit->getType().isWorker())
	{
		return new WorkerAgent(unit);
	}
	else if (unit->getType().isBuilding())
	{
		//Add agents for special buildings here
		if (isOfType(unit, UnitTypes::Protoss_Nexus))
		{
			return new NexusAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Assimilator))
		{
			return new RefineryAgent(unit);
		}
		else
		{
			//Default structure agent
			return new StructureAgent(unit);
		}
	}
	else
	{
#if DISABLE_UNIT_AI == 0
		if (isOfType(unit, UnitTypes::Protoss_Zealot))
		{
			return new ZealotAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Dragoon))
		{
			return new DragoonAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Reaver))
		{
			return new ReaverAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Observer))
		{
			return new ObserverAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Dark_Templar))
		{
			return new DarkTemplarAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Scout))
		{
			return new ScoutAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Shuttle))
		{
			return new TransportAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Corsair))
		{
			return new CorsairAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Carrier))
		{
			return new CarrierAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_High_Templar))
		{
			return new HighTemplarAgent(unit);
		}
		else
		{
			//Default unit agent
			return new UnitAgent(unit);
		}
#else
		return new UnitAgent(unit);
#endif
	}
	return NULL;
}

bool AgentFactory::isOfType(Unit* unit, UnitType type)
{
	if (unit->getType().getID() == type.getID())
	{
		return true;
	}
	return false;
}

