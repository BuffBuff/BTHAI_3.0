#include "UpgradesFileReader.h"
#include "FileReaderUtils.h"
#include "ExplorationManager.h"
#include "BuildPlanner.h"
#include <fstream>
#include <sstream>

UpgradesFileReader::UpgradesFileReader()
{

}

vector<UpgradeType> UpgradesFileReader::getUpgradesP1()
{
	return upgradesP1;
}

vector<UpgradeType> UpgradesFileReader::getUpgradesP2()
{
	return upgradesP2;
}

vector<UpgradeType> UpgradesFileReader::getUpgradesP3()
{
	return upgradesP3;
}

vector<TechType> UpgradesFileReader::getTechsP1()
{
	return techsP1;
}

vector<TechType> UpgradesFileReader::getTechsP2()
{
	return techsP2;
}

vector<TechType> UpgradesFileReader::getTechsP3()
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
	
	UpgradeType type = getUpgradeType(tokens.key);
	if (type.getID() != UpgradeTypes::Unknown.getID())
	{
		int value = toInt(tokens.value);
		if (value == 1) upgradesP1.push_back(type);
		if (value == 2) upgradesP2.push_back(type);
		if (value == 3) upgradesP2.push_back(type);
		return;
	}
	
	TechType ttype = getTechType(tokens.key);
	if (ttype.getID() != TechTypes::Unknown.getID())
	{
		int value = toInt(tokens.value);
		if (value == 1) techsP1.push_back(ttype);
		if (value == 2) techsP1.push_back(ttype);
		if (value == 3) techsP1.push_back(ttype);
		return;
	}

	//No match found
	Broodwar->printf("Error: No matching upgrade or tech found for %s", line.c_str());
}
