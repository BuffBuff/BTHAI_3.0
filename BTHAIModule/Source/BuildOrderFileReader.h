#ifndef __BUILDORDERFILEREADER_H__
#define __BUILDORDERFILEREADER_H__

#include <BWAPI.h>
#include "FileReaderUtils.h"

using namespace BWAPI;
using namespace std;

struct BuildPlan
{
	UnitType type;
	int frameDelay;
};

/** This file reads the buildorder script files.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class BuildOrderFileReader : public FileReaderUtils {

private:
	void addUnitType(string line, vector<BuildPlan> &buildOrder);

public:
	BuildOrderFileReader();

	/** Reads the buildorder from script file. */
	vector<BuildPlan> readBuildOrder();
};

#endif
