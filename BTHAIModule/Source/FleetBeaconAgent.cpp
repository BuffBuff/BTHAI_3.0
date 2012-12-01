#include "FleetBeaconAgent.h"
#include "AgentManager.h"

FleetBeaconAgent::FleetBeaconAgent(Unit* mUnit)
{
	unit = mUnit;
	unitID = unit->getID();
	//Broodwar->printf("FleetBeaconAgent created (%s)", unit->getType().getName().c_str());

	researchOrder.push_back(TechTypes::Disruption_Web);
}

void FleetBeaconAgent::computeActions()
{
	doResearch();
}

string FleetBeaconAgent::getTypeName()
{
	return "FleetBeaconAgent";
}



