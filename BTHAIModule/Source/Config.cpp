#include "Config.h"
#include "BuildPlanner.h"
#include "ExplorationManager.h"
#include <fstream>
#include <sstream>

Config* Config::instance = NULL;

Config::Config()
{
	version = "";
	scriptPath = "bwapi-data\\AI\\BTHAI-data\\";
	botName = "BTHAI";
	init = false;

	readConfigFile();

	stringstream s1;
	s1 << botName;
	s1 << " ";
	s1 << version;
	info1 = s1.str();

	stringstream s2;
	s2 << "";
	if (botName != "BTHAI")
	{
		s2 << "Using BTHAI engine";
	}
	info2 = s2.str();

}

Config::~Config()
{
	delete instance;
}

Config* Config::getInstance()
{
	if (instance == NULL)
	{
		instance = new Config();
	}
	return instance;
}

void Config::readConfigFile()
{
	//Read config file
	ifstream inFile;
	inFile.open("bwapi-data\\AI\\BTHAI-data\\bthai-config.txt");
	if (inFile)
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
			
				CTokens token = split(line, "=");
				if (token.key == "ScriptPath")
				{
					scriptPath = token.value;

					int index = scriptPath.rfind("\\");
					if (index != scriptPath.length() - 1)
					{
						stringstream st;
						st << scriptPath;
						st << "\\";
						scriptPath = st.str();
					}
				}
				if (token.key == "BotName")
				{
					botName = token.value;
				}
				if (token.key == "Version")
				{
					version = token.value;
				}
			}
		}

		inFile.close();
	}

	init = true;
}

void Config::displayBotName()
{
	if (Broodwar->getFrameCount() >= 10)
	{
		Broodwar->drawTextScreen(510,26, info1.c_str());
		Broodwar->drawTextScreen(510,44, info2.c_str());
	}
}

string Config::getVersion()
{
	return version;
}

string Config::getBotName()
{
	return botName;
}

string Config::getScriptPath()
{
	if (!init)
	{
		readConfigFile();
	}

	return scriptPath;
}

int Config::toInt(string &str)
{
	stringstream ss(str);
	int n;
	ss >> n;
	return n;
}

CTokens Config::split(string line, string delimiter)
{
	CTokens tokens;
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
