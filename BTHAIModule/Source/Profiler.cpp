#include "Profiler.h"
#include <iostream>
#include <fstream>

bool Profiler::instanceFlag = false;
Profiler* Profiler::instance = NULL;

Profiler::Profiler()
{
	
}

Profiler::~Profiler()
{
	for (int i = 0; i < (int)obj.size(); i++)
	{	
		delete obj.at(i);
	}
}

Profiler* Profiler::getInstance()
{
	if (!instanceFlag)
	{
		instance = new Profiler();
		instanceFlag = true;
	}
	return instance;
}

ProfilerObj* Profiler::getObj(string mId)
{
	for (int i = 0; i < (int)obj.size(); i++)
	{
		if (obj.at(i)->matches(mId))
		{
			return obj.at(i);
		}
	}
	return NULL;
}


void Profiler::start(string mId)
{
	ProfilerObj* cObj = getObj(mId);
	if (cObj != NULL) 
	{
		cObj->start();
	}
	else
	{
		ProfilerObj* newObj = new ProfilerObj(mId);
		newObj->start();
		obj.push_back(newObj);
	}
}

void Profiler::end(string mId)
{
	ProfilerObj* cObj = getObj(mId);
	if (cObj != NULL) cObj->end();
}

int Profiler::getElapsed(string mId) 
{
	ProfilerObj* cObj = getObj(mId);
	if (cObj != NULL) return cObj->getElapsed();
	return 0;
}

void Profiler::show(string mId)
{
	ProfilerObj* cObj = getObj(mId);
	if (cObj != NULL) cObj->show();
}

void Profiler::showAll()
{
	for (int i = 0; i < (int)obj.size(); i++)
	{
		obj.at(i)->show();
	}
}

void Profiler::dumpToFile()
{
	ofstream ofile;
	ofile.open("bwapi-data\\AI\\BTHAI-data\\Profiling.html");

	ofile << "<html><head>";
	ofile << "<link rel='stylesheet' type='text/css' href='style.css' />";
	ofile << "<title>BTHAI Profiler</title></head><body>\n";
	ofile << "<table>";

	ofile << "<tr><td class='h'>";
	ofile << "Id";
	ofile << "</td><td class='h'>";
	ofile << "AvgFrameTime";
	ofile << "</td><td class='h'>";
	ofile << "Calls";
	ofile << "</td><td class='h'>";
	ofile << "MaxTime";
	ofile << "</td><td class='h'>";
	ofile << "TO_1min";
	ofile << "</td><td class='h'>";
	ofile << "TO_1sec";
	ofile << "</td><td class='h'>";
	ofile << "TO_55ms";
	ofile << "</td>";
	ofile << "</tr>\n";

	for (int i = 0; i < (int)obj.size(); i++)
	{
		ofile << obj.at(i)->getDumpStr().c_str();
	}

	ofile << "</table>";

	//Test
	/*ofile << "\n";
	ofile << "<table>\n";
	for(set<UnitType>::iterator i=UnitTypes::allUnitTypes().begin();i!=UnitTypes::allUnitTypes().end();i++)
	{
		string name = (*i).getName();
		if (name.find("Terran") == 0 || name.find("Protoss") == 0 || name.find("Zerg") == 0)
		{
			ofile << "<tr>\n";
			ofile << "<td class='t'>\n";
			ofile << (*i).getName();
			ofile << "</td><td class='t'>\n";
			if ((*i).isBuilding()) 
			{
				ofile << "Building";
			}
			else if ((*i).isAddon()) 
			{
				ofile << "Addon";
			}
			else
			{
				ofile << "Unit";
			}
			ofile << "</td><td class='t'>\n";
			ofile << (*i).getRace().getName();

			ofile << "</td><td class='t'>\n";

			if ((*i).isBuilding())
			{
				map<UnitType,int> reqs = (*i).requiredUnits();
				for(map<UnitType,int>::iterator j=reqs.begin();j!=reqs.end();j++)
				{
					if ((*j).first.isBuilding() && !(*j).first.isResourceDepot() && !(*j).first.isAddon())
					{
						ofile << (*j).first.getName();
					}
				}
			}
			else if ((*i).isAddon()) 
			{
				//Do nothing
			}
			else
			{
				if ((*i).isFlyer())
				{
					ofile << "Air";
				}
				else
				{
					ofile << "Ground";
				}
			}
			ofile << "</td>\n";
			ofile << "</tr>\n";
		}
	}
	ofile << "</table>\n";
	ofile << "\n\n";

	ofile << "<table>\n";
	for(set<UpgradeType>::iterator i=UpgradeTypes::allUpgradeTypes().begin();i!=UpgradeTypes::allUpgradeTypes().end();i++)
	{
		ofile << "<tr>\n";
		ofile << "<td class='t'>\n";
		ofile << (*i).getName();
		ofile << "</td><td class='t'>\n";
		ofile << "Upgrade";
		ofile << "</td><td class='t'>\n";
		ofile << (*i).getRace().getName();
		ofile << "</td>\n";
		ofile << "</tr>\n";
	}
	ofile << "</table>\n";
	ofile << "\n\n";

	ofile << "<table>\n";
	for(set<TechType>::iterator i=TechTypes::allTechTypes().begin();i!=TechTypes::allTechTypes().end();i++)
	{
		ofile << "<tr>\n";
		ofile << "<td class='t'>\n";
		ofile << (*i).getName();
		ofile << "</td><td class='t'>\n";
		ofile << "Tech";
		ofile << "</td><td class='t'>\n";
		ofile << (*i).getRace().getName();
		ofile << "</td>\n";
	}
	ofile << "</table>\n";
	ofile << "\n\n";
	//End Test*/

	ofile << "</body></html>";

	ofile.close();
}