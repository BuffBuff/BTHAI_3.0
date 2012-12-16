#include "PathObj.h"


PathObj::PathObj(TilePosition cStart, TilePosition cEnd)
{
	start = cStart;
	end = cEnd;
	finished = false;
}

PathObj::~PathObj()
{
	
}

bool PathObj::matches(TilePosition cStart, TilePosition cEnd)
{
	if (cStart.x() != start.x()) return false;
	if (cStart.y() != start.y()) return false;
	if (cEnd.x() != end.x()) return false;
	if (cEnd.y() != end.y()) return false;
	return true;
}

bool PathObj::isFinished()
{
	return finished;
}

void PathObj::calculatePath() 
{
	if (!start.isValid() || !end.isValid())
		Broodwar->printf("Requested path finding from (%d, %d) to (%d, %d), which is invalid", start.x(), start.y(), end.x(), end.y());
	else
		path = getShortestPath(start, end);

	finished = true;
}

vector<TilePosition> PathObj::getPath()
{
	return path;
}
