#include "FileReaderUtils.h"
#include "BuildPlanner.h"
#include "ExplorationManager.h"
#include "Config.h"
#include <fstream>
#include <sstream>

FileReaderUtils::FileReaderUtils()
{
	
}

string FileReaderUtils::getFilename(string subpath)
{
	string filename = "";

	if (BuildPlanner::isProtoss())
	{
		if (ExplorationManager::enemyIsProtoss())
		{
			filename = "PvP.txt";
			if (!fileExists(subpath, filename)) filename = "PvX.txt";
		}
		else if (ExplorationManager::enemyIsTerran())
		{
			filename = "PvT.txt";
			if (!fileExists(subpath, filename)) filename = "PvX.txt";
		}
		else if (ExplorationManager::enemyIsZerg())
		{
			filename = "PvZ.txt";
			if (!fileExists(subpath, filename)) filename = "PvX.txt";
		}
	}
	else if (BuildPlanner::isTerran())
	{
		if (ExplorationManager::enemyIsProtoss())
		{
			filename = "TvP.txt";
			if (!fileExists(subpath, filename)) filename = "TvX.txt";
		}
		else if (ExplorationManager::enemyIsTerran())
		{
			filename = "TvT.txt";
			if (!fileExists(subpath, filename)) filename = "TvX.txt";
		}
		else if (ExplorationManager::enemyIsZerg())
		{
			filename = "TvZ.txt";
			if (!fileExists(subpath, filename)) filename = "TvX.txt";
		}
	}
	else if (BuildPlanner::isZerg())
	{
		if (ExplorationManager::enemyIsProtoss())
		{
			filename = "ZvP.txt";
			if (!fileExists(subpath, filename)) filename = "ZvX.txt";
		}
		else if (ExplorationManager::enemyIsTerran())
		{
			filename = "ZvT.txt";
			if (!fileExists(subpath, filename)) filename = "ZvX.txt";
		}
		else if (ExplorationManager::enemyIsZerg())
		{
			filename = "ZvZ.txt";
			if (!fileExists(subpath, filename)) filename = "ZvX.txt";
		}
	}
	else
	{

	}
	return filename;
}

string FileReaderUtils::getScriptPath()
{
	return Config::getInstance()->getScriptPath();
}

bool FileReaderUtils::fileExists(string subpath, string filename)
{
	ifstream inFile;

	stringstream ss;
	ss << getScriptPath();
	ss << subpath;
	ss << "\\";
	ss << filename;
	string filePath = ss.str();

	inFile.open(filePath.c_str());

	if (!inFile)
	{
		return false;
	}
	else
	{
		inFile.close();
		return true;
	}
}

UnitType FileReaderUtils::getUnitType(string line)
{
	if (line == "") return UnitTypes::Unknown;

	//Replace all _ with whitespaces, or they wont match
	replace(line);
	
	for(set<UnitType>::iterator i=UnitTypes::allUnitTypes().begin();i!=UnitTypes::allUnitTypes().end();i++)
	{
		if ((*i).getName() == line)
		{
			return (*i);
		}
	}

	//No UnitType match found
	Broodwar->printf("Error: No matching UnitType found for %s", line.c_str());
	return UnitTypes::Unknown;
}

UpgradeType FileReaderUtils::getUpgradeType(string line)
{
	if (line == "") return UpgradeTypes::Unknown;

	//Replace all _ with whitespaces, or they wont match
	replace(line);
	
	for(set<UpgradeType>::iterator i=UpgradeTypes::allUpgradeTypes().begin();i!=UpgradeTypes::allUpgradeTypes().end();i++)
	{
		if ((*i).getName() == line)
		{
			return (*i);
		}
	}

	//No UnitType match found
	return UpgradeTypes::Unknown;
}

TechType FileReaderUtils::getTechType(string line)
{
	if (line == "") return TechTypes::Unknown;

	//Replace all _ with whitespaces, or they wont match
	replace(line);
	
	for(set<TechType>::iterator i=TechTypes::allTechTypes().begin();i!=TechTypes::allTechTypes().end();i++)
	{
		if ((*i).getName() == line)
		{
			return (*i);
		}
	}

	//No UnitType match found
	Broodwar->printf("Error: No matching TechType found for %s", line.c_str());
	return TechTypes::Unknown;
}

void FileReaderUtils::replace(string &line)
{
	int usIndex = line.find("_");
	while (usIndex != string::npos)
	{
		line.replace(usIndex, 1, " ");
		usIndex = line.find("_");
	}
}

int FileReaderUtils::toInt(string &str)
{
	stringstream ss(str);
	int n;
	ss >> n;
	return n;
}

Tokens FileReaderUtils::split(string line, string delimiter)
{
	Tokens tokens;
	tokens.key = "";
	tokens.value = "";

	int eqIndex = line.find(delimiter);
	if (eqIndex != string::npos)
	{
		tokens.key = line.substr(0, eqIndex);
		tokens.value = line.substr(eqIndex + 1, line.length());
	}
	return tokens;
}
