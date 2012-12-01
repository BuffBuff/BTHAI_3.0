#ifndef __SQUADFILEREADER_H__
#define __SQUADFILEREADER_H__

#include <BWAPI.h>
#include "Squad.h"
#include "FileReaderUtils.h"

using namespace BWAPI;
using namespace std;

/** This class reads the squad setup script files.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class SquadFileReader : public FileReaderUtils {

private:
	void addSquad(string line, vector<UnitType> &squads);
	void addUnit(string line);
	void createSquad();

	string type;
	string name;
	string offType;
	string requirement;
	int priority;
	int activePriority;
	UnitType morphsTo;

	Squad* cSquad;
	int id;

	vector<Squad*> squads;

public:
	/** Constructor. */
	SquadFileReader();

	/** Reads the squad setup script file. */
	vector<Squad*> readSquadList();
};

#endif
