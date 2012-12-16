#include "UpgradesFileReader.h"
#include "FileReaderUtils.h"
#include "ExplorationManager.h"
#include "BuildPlanner.h"
#include <fstream>
#include <sstream>

UpgradesFileReader::UpgradesFileReader()
{

}

vector<UpgradePlan> UpgradesFileReader::getUpgradesP1()
{
	return upgradesP1;
}

vector<UpgradePlan> UpgradesFileReader::getUpgradesP2()
{
	return upgradesP2;
}

vector<UpgradePlan> UpgradesFileReader::getUpgradesP3()
{
	return upgradesP3;
}

vector<TechPlan> UpgradesFileReader::getTechsP1()
{
	return techsP1;
}

vector<TechPlan> UpgradesFileReader::getTechsP2()
{
	return techsP2;
}

vector<TechPlan> UpgradesFileReader::getTechsP3()
{
	return techsP3;
}

void UpgradesFileReader::readUpgrades()
{
	string filename = getFilename("upgrades");
	
	ifstream inFile;

	stringstream ss;
	ss << getScriptPath();
	ss << "upgrades\\";
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
				addUpgrade(line);
			}
		}
		inFile.close();
	}

	Broodwar->printf("Upgrades file %s loaded", filePath.c_str());
}

void UpgradesFileReader::addUpgrade(string line)
{
	if (line == "") return;

	//Replace all _ with whitespaces, or they wont match
	replace(line);
	Tokens tokens = split(line, ":");

	if (tokens.value == "")
		return;

	Tokens values = split(tokens.value, ",");

	int value = toInt(values.key);
	
	int frameDelay = 0;
	if (values.value != "")
		frameDelay = toInt(values.value);

	UpgradeType type = getUpgradeType(tokens.key);
	if (type.getID() != UpgradeTypes::Unknown.getID())
	{
		UpgradePlan upPlan = {type, frameDelay};
		if (value == 1) upgradesP1.push_back(upPlan);
		if (value == 2) upgradesP2.push_back(upPlan);
		if (value == 3) upgradesP2.push_back(upPlan);
		return;
	}
	
	TechType ttype = getTechType(tokens.key);
	if (ttype.getID() != TechTypes::Unknown.getID())
	{
		TechPlan techPlan = {ttype, frameDelay};
		if (value == 1) techsP1.push_back(techPlan);
		if (value == 2) techsP1.push_back(techPlan);
		if (value == 3) techsP1.push_back(techPlan);
		return;
	}

	//No match found
	Broodwar->printf("Error: No matching upgrade or tech found for %s", line.c_str());
}
