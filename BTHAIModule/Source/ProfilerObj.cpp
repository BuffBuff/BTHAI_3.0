#include "ProfilerObj.h"
#include <sstream>

ProfilerObj::ProfilerObj(string mId)
{
	startTime = 0;
	endTime = 0;
	id = mId;
	total = 0;
	maxTime = 0;
	startCalls = 0;
	endCalls = 0;
	lastShowFrame = 0;

	timeouts_short = 0;
	timeouts_medium = 0;
	timeouts_long = 0;
}

ProfilerObj::~ProfilerObj()
{
	
}

bool ProfilerObj::matches(string mId)
{
	return mId == id;
}

void ProfilerObj::start()
{
	startTime = (int)GetTickCount();
	startCalls++;
}

void ProfilerObj::end()
{
	endTime = (int)GetTickCount();

	int elapsed = getElapsed();
	total += elapsed;
	endCalls++;

	if (elapsed >= 60000) timeouts_long++;
	if (elapsed >= 1000) timeouts_medium++;
	if (elapsed >= 55) timeouts_short++;
	if (elapsed > maxTime) maxTime = elapsed;
}

int ProfilerObj::getElapsed()
{
	return endTime - startTime;
}

void ProfilerObj::show()
{
	if (Broodwar->getFrameCount() - lastShowFrame < 400) return;

	lastShowFrame = Broodwar->getFrameCount();

	double avg = (double)total / (double)endCalls;

	Broodwar->printf("[%s] AvgFrame: %d  MaxFrame: %d  TO_1min: %d  TO_1sec: %d  TO_55ms: %d", id.c_str(), (int)avg, maxTime, timeouts_long, timeouts_medium, timeouts_short);
	if (timeouts_long >= 1 || timeouts_medium >= 10 || timeouts_short >= 320)
	{
		Broodwar->printf("[%s] Timeout fail!!!", id.c_str());
	}
	if (startCalls != endCalls)
	{
		Broodwar->printf("[%s] Warning! Start- and endcalls mismatch %d/%d", id.c_str(), startCalls, endCalls);
	}
}

string ProfilerObj::getDumpStr()
{
	int iAvg = (int)((double)total / (double)endCalls * (double)100);
	double avg = (double)iAvg / (double)100;

	stringstream ss;
	
	ss << "<tr><td>";
	ss << id;
	ss << "</td><td>";
	ss << avg;
	ss << "</td><td>";
	if (startCalls == endCalls)
	{
		ss << endCalls;
	}
	else
	{
		ss << "Calls missmatch (";
		ss << startCalls;
		ss << "/";
		ss << endCalls;
		ss << ")";
	}
	ss << "</td><td>";
	ss << maxTime;
	ss << "</td><td>";
	ss << timeouts_long;
	ss << "</td><td>";
	ss << timeouts_medium;
	ss << "</td><td>";
	ss << timeouts_short;
	ss << "</td>";
	ss << "</tr>\n";
	
	return ss.str();
}