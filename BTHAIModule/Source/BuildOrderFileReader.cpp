#include "BuildOrderFileReader.h"
#include "ExplorationManager.h"
#include "BuildPlanner.h"
#include <fstream>
#include <sstream>

BuildOrderFileReader::BuildOrderFileReader()
{
	
}

vector<BuildPlan> BuildOrderFileReader::readBuildOrder()
{
	string filename = getFilename("buildorder");
	vector<BuildPlan> buildOrder;

	//Read buildorder file
	ifstream inFile;

	stringstream ss;
	ss << getScriptPath();
	ss << "buildorder\\";
	ss << filename;
	string filePath = ss.str();

	inFile.open(filePath.c_str());

	if (!inFile)
	{
		Broodwar->printf("Unable to open file %s", filePath.c_str());
	}
	else
	{
		string line;
		char buffer[256];
		while (!inFile.eof())
		{
			inFile.getline(buffer, 100);
			if (buffer[0] != ';')
			{
				stringstream ss;
				ss << buffer;
				line = ss.str();
				addUnitType(line, buildOrder);
			}
		}
		inFile.close();
	}

	Broodwar->printf("Build order file %s loaded", filePath.c_str());

	return buildOrder;
}

void BuildOrderFileReader::addUnitType(string line, vector<BuildPlan> &buildOrder)
{
	if (line == "") return;

	//Replace all _ with whitespaces, or they wont match
	replace(line);

	Tokens token = split(line, ":");

	int frameDelay = 0;
	if (!token.value.empty())
	{
		frameDelay = toInt(token.value);
	}
	
	for(set<UnitType>::iterator i=UnitTypes::allUnitTypes().begin();i!=UnitTypes::allUnitTypes().end();i++)
	{
		if ((*i).getName() == token.key)
		{
			BuildPlan bPlan = { (*i), frameDelay };

			buildOrder.push_back(bPlan);
			return;
		}
	}

	//No UnitType match found
	Broodwar->printf("Error: No matching UnitType found for %s", line.c_str());
}
