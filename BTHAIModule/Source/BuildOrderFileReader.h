#ifndef __BUILDORDERFILEREADER_H__
#define __BUILDORDERFILEREADER_H__

#include <BWAPI.h>
#include "FileReaderUtils.h"

using namespace BWAPI;
using namespace std;

/** This file reads the buildorder script files.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class BuildOrderFileReader : public FileReaderUtils {

private:
	void addUnitType(string line, vector<UnitType> &buildOrder);

public:
	BuildOrderFileReader();

	/** Reads the buildorder from script file. */
	vector<UnitType> readBuildOrder();
};

#endif
