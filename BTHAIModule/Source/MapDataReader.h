#ifndef __MAPDATAREADER_H__
#define __MAPDATAREADER_H__

#include "BaseAgent.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

struct MapData {
	TilePosition basePos;
	TilePosition pos;
	int dist;

	bool matches(TilePosition t1, TilePosition t2)
	{
		if (t1.x() == basePos.x() && t1.y() == basePos.y() && t2.x() == pos.x() && t2.y() == pos.y())
		{
			return true;
		}
		return false;
	}
};

/** This class handles load/save of additional mapdata not contained in bwapi/bwta. This
 * is used to quick calculate ground distances between important regions like start positions.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class MapDataReader {

private:
	vector<MapData> data;
	string mapName;

	void saveFile();
	bool loadFile();
	string getFilename();
	void addEntry(string line);
	int toInt(string &str);

public:
	/** Constructor */
	MapDataReader();

	/** Destructor */
	~MapDataReader();

	/** Reads the map file. If no file is found, a new is generated. */
	void readMap();

	/** Returns the ground distance between two positions. If not found in
	 * the data, bird distance is used. */
	int getDistance(TilePosition t1, TilePosition t2);
};

#endif
