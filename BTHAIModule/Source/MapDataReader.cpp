#include "MapDataReader.h"
#include "ExplorationManager.h"
#include <fstream>
#include <sstream>

MapDataReader::MapDataReader()
{
	
}

MapDataReader::~MapDataReader()
{
	
}

int MapDataReader::getDistance(TilePosition t1, TilePosition t2)
{
	for (int i = 0; i < (int)data.size(); i++)
	{
		if (data.at(i).matches(t1, t2))
		{
			return data.at(i).dist;
		}
	}
	int dist = (int)(t1.getDistance(t2));
	return dist;
}

void MapDataReader::readMap()
{
	if (loadFile())
	{
		//Map data loaded successfully
		return;
	}

	if (!analyzed)
	{
		return;
	}

	for(set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin(); i!= BWTA::getBaseLocations().end(); i++)
	{
		TilePosition basePos = (*i)->getTilePosition();
		for(set<BWTA::BaseLocation*>::const_iterator j=BWTA::getBaseLocations().begin(); j!= BWTA::getBaseLocations().end(); j++)
		{
			TilePosition pos = (*j)->getTilePosition();
			int cDist = -1;
			if (ExplorationManager::canReach(basePos, pos))
			{
				cDist = (int)(getGroundDistance(basePos, pos));
			}

			MapData entry;
			entry.basePos = basePos;
			entry.pos = pos;
			entry.dist = cDist;

			data.push_back(entry);
		}
	}

	saveFile();
}

bool MapDataReader::loadFile()
{
	string filename = getFilename();

	ifstream inFile;
	inFile.open(filename.c_str());
	if (!inFile)
	{
		//Map data not found. Generate new.
		return false;
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
				addEntry(line);
			}
		}
		inFile.close();
	}
	return true;
}

void MapDataReader::addEntry(string line)
{
	int i = line.find(" ");
	string t1 = line.substr(0, i);
	line = line.substr(i + 1, line.length());

	i = line.find(" ");
	string t2 = line.substr(0, i);
	line = line.substr(i + 1, line.length());

	i = line.find(" ");
	string t3 = line.substr(0, i);
	line = line.substr(i + 1, line.length());

	i = line.find(" ");
	string t4 = line.substr(0, i);
	line = line.substr(i + 1, line.length());

	string t5 = line;

	//Broodwar->printf("'%s' '%s' '%s' '%s' '%s'", t1.c_str(), t2.c_str(), t3.c_str(), t4.c_str(), t5.c_str());

	MapData entry;
	entry.basePos = TilePosition(toInt(t1), toInt(t2));
	entry.pos = TilePosition(toInt(t3), toInt(t4));
	entry.dist = toInt(t5);
	
	data.push_back(entry);
}

int MapDataReader::toInt(string &str)
{
	stringstream ss(str);
	int n;
	ss >> n;
	return n;
}

string MapDataReader::getFilename()
{
	stringstream ss;
	ss << "bwapi-data\\AI\\BTHAI-data\\mapdata\\";
	ss << Broodwar->mapHash();
	ss << ".txt";

	return ss.str();
}

void MapDataReader::saveFile()
{
	string filename = getFilename();

	ofstream outFile;
	outFile.open(filename.c_str());
	if (!outFile)
	{
		Broodwar->printf("Unable to write data to %s", filename.c_str());
	}
	else
	{
		for (int i = 0; i < (int)data.size(); i++)
		{
			stringstream s2;
			s2 << data.at(i).basePos.x();
			s2 << " ";
			s2 << data.at(i).basePos.y();
			s2 << " ";
			s2 << data.at(i).pos.x();
			s2 << " ";
			s2 << data.at(i).pos.y();
			s2 << " ";
			s2 << data.at(i).dist;
			s2 << "\n";

			outFile << s2.str();
		}
		outFile.close();
	}
}

/*double CoverMap::getDistTo(TilePosition pos)
{
	//double dist = BWTA::getGroundDistance(Broodwar->self()->getStartLocation(), pos); //Very slow but more accurate
	double dist = Broodwar->self()->getStartLocation().getDistance(pos); //Fast, but less accurate

	//Test
	Region* cRegion = getRegion(pos);
	if (cRegion != NULL)
	{
		for(set<Region*>::const_iterator i=cRegion->getReachableRegions().begin();i!=cRegion->getReachableRegions().end();i++)
		{
			if (isOccupied((*i)))
			{
				dist = dist * 0.7;
				break;
			}
		}
	}

	return dist;
}*/
