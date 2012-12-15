#include "Pathfinder.h"
#include "ExplorationManager.h"
#include "Profiler.h"
#include <fstream>
#include <sstream>

bool Pathfinder::instanceFlag = false;
Pathfinder* Pathfinder::instance = NULL;

Pathfinder::Pathfinder()
{
	running = true;
	lock = false;
	end = false;
	//CreateThread();
}

Pathfinder::~Pathfinder()
{
	stop();

	for (int i = 0; i < (int)pathObj.size(); i++)
	{	
		delete pathObj.at(i);
	}
}

Pathfinder* Pathfinder::getInstance()
{
	if (!instanceFlag)
	{
		instance = new Pathfinder();
		instanceFlag = true;
	}
	return instance;
}

PathObj* Pathfinder::getPathObj(TilePosition start, TilePosition end)
{
	for (int i = 0; i < (int)pathObj.size(); i++) {
		if (pathObj.at(i)->matches(start, end))
		{
			return pathObj.at(i);
		}
	}
	return NULL;
}

int Pathfinder::getDistance(TilePosition start, TilePosition end)
{
	PathObj* obj = getPathObj(start, end);
	if (obj != NULL)
	{
		if (obj->isFinished())
		{
			return obj->getPath().size();
		}
	}
	return 0;
}

void Pathfinder::requestPath(TilePosition start, TilePosition end)
{
	PathObj* obj = getPathObj(start, end);
	if (obj == NULL) 
	{
		obj = new PathObj(start, end);
		obj->calculatePath();
		pathObj.push_back(obj);
	}

	const static unsigned long CACHE_SIZE = 50;
	if ((int)pathObj.size() > CACHE_SIZE)
	{
		lock = true;

		while (pathObj.size() > CACHE_SIZE)
		{
			pathObj.erase(pathObj.begin());
		}

		lock = false;
	}
}

bool Pathfinder::isReady(TilePosition start, TilePosition end)
{
	PathObj* obj = getPathObj(start, end);
	if (obj != NULL)
	{
		return obj->isFinished();
	}
	return false;
}

vector<TilePosition> Pathfinder::getPath(TilePosition start, TilePosition end)
{
	PathObj* obj = getPathObj(start, end);
	if (obj != NULL)
	{
		if (obj->isFinished())
		{
			return obj->getPath();
		}
	}
	return vector<TilePosition>();
}

void Pathfinder::stop()
{
	running = false;

	//WaitForSingleObject(GetThreadHandle(), INFINITE);
}

bool Pathfinder::isRunning()
{
	if (!Broodwar->isInGame()) running = false;
	return running;
}

unsigned long Pathfinder::Process (void* parameter)
{
	while (running)
	{
		for (int i = 0; i < (int)pathObj.size(); i++)
		{
			if (!isRunning()) break;
			while (lock);
			if (!isRunning()) break;
			if (!pathObj.at(i)->isFinished())
			{
				if (!isRunning()) break;
				pathObj.at(i)->calculatePath();
			}
		}
		Sleep(50);
	}

	end = true;

	return 0;
}

