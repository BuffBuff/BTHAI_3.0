#include "Squad.h"
#include "UnitAgent.h"
#include "AgentManager.h"
#include "ExplorationManager.h"
#include "Commander.h"
#include "BuildPlanner.h"
#include "Pathfinder.h"
#include "Profiler.h"

Squad::Squad()
{

}

Squad::Squad(int mId, int mType, string mName, int mPriority)
{
	this->id = mId;
	this->type = mType;
	this->moveType = AIR;
	this->name = mName;
	this->priority = mPriority;
	morphs = UnitTypes::Unknown;
	activePriority = priority;
	active = false;
	required = false;
	goal = TilePosition(-1, -1);
	goalSetFrame = 0;
	arrivedFrame = -1;
	currentState = STATE_DEFEND;
}

string Squad::getName()
{
	return name;
}

UnitType Squad::morphsTo()
{
	return morphs;
}

void Squad::setMorphsTo(UnitType type)
{
	morphs = type;
}

int Squad::getID()
{
	return id;
}

bool Squad::isRequired()
{
	return required;
}

void Squad::setRequired(bool mRequired)
{
	required = mRequired;
}

int Squad::getPriority()
{
	return priority;
}

void Squad::setPriority(int mPriority)
{
	priority = mPriority;
}

void Squad::setActivePriority(int mPriority)
{
	activePriority = mPriority;
}

bool Squad::isActive()
{
	return active;
}

void Squad::forceActive()
{
	int noAlive = 0;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			noAlive++;
		}
	}

	if (noAlive > 0)
	{
		active = true;
	}
}

int Squad::size()
{
	int no = 0;
	for(int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			no++;
		}
	}
	return no;
}

void Squad::addSetup(UnitType type, int no)
{
	UnitSetup us;
	us.type = type;
	us.no = no;
	us.current = 0;
	setup.push_back(us);

	if (!type.isFlyer())
	{
		moveType = GROUND;
	}
}

vector<UnitSetup> Squad::getSetup()
{
	return setup;
}

void Squad::debug_showGoal()
{
	if (size() > 0 && goal.x() >= 0)
	{
		Position a = Position(goal.x() * 32 - 3, goal.y() * 32 - 3);

		Broodwar->drawCircle(CoordinateType::Map, a.x(), a.y(), 6, Colors::Grey, true);
		Broodwar->drawText(CoordinateType::Map, a.x() - 20, a.y() - 5, "SQ %d: (%d/%d)", id, getSize(), getTotalUnits());
	}
}

void Squad::computeActions()
{
	if (!active)
	{
		if (isFull())
		{
			active = true;
		}
	}

	if (active)
	{
		int noAlive = 0;
		int noDead = 0;
		for (int i = 0; i < (int)agents.size(); i++)
		{
			if (agents.at(i)->isAlive())
			{
				noAlive++;
			}
			if (!agents.at(i)->isAlive())
			{
				noDead++;
			}
		}

		if ((int)agents.size() > 0)
		{
			if (noAlive <= ((int)agents.size() / 10))
			{
				noAlive = 0;
			}
		}

		if (noAlive == 0)
		{
			active = false;
			clearGoal();
			return;
		}
	}

	if (active)
	{
		if (activePriority != priority)
		{
			priority = activePriority;
		}
	}

	checkAttack();
}

bool Squad::isAttacking()
{
	if (!isExplorer())
	{
		Unit* target = findTarget();
		if (target != NULL)
		{
			return true;
		}
	}
	return false;
}

void Squad::checkAttack()
{
	if (!isExplorer())
	{
		Unit* target = findTarget();
		
		if (target != NULL)
		{
			for (int i = 0; i < (int)agents.size(); i++)
			{
				BaseAgent* agent = agents.at(i);
				if (agent->isAlive() && agent->canAttack(target) && !(agent->getUnit()->isAttacking() || agent->getUnit()->isStartingAttack()))
				{
					agent->getUnit()->attack(target);
				}
			}
		}
	}
}

Unit* Squad::findTarget()
{
	try {
		//Enemy units
		for (int i = 0; i < (int)agents.size(); i++)
		{
			BaseAgent* agent = agents.at(i);
			int maxRange = agent->getUnitType().seekRange();
			if (agent->isAlive())
			{
				for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
				{
					if ((*i)->exists())
				{
						double dist = agent->getUnit()->getDistance((*i));
				
						//Check if enemy unit is cloaked
						if (dist <= maxRange)
				{
							if ((*i)->isCloaked())
				{
								Commander::getInstance()->handleCloakedEnemy((*i)->getTilePosition(), this);
							}

							return (*i);
						}	
					}
				}
			}
		}

		//Neutral units
		/*for (int i = 0; i < (int)agents.size(); i++)
		{
			BaseAgent* agent = agents.at(i);
			for(set<Unit*>::const_iterator i=Broodwar->neutral()->getUnits().begin();i!=Broodwar->neutral()->getUnits().end();i++)
			{
				if ((*i)->exists())
				{
					if ((*i)->getType().getID() == UnitTypes::Special_Power_Generator.getID())
				{
						double dist = agent->getUnit()->getDistance((*i));
				
						if (dist <= maxRange)
				{
							agent->getUnit()->attack((*i));
							return (*i);
						}
					}
				}
			}
		}*/

	}
	catch (exception)
	{

	}

	return NULL;
}

bool Squad::isUnderAttack()
{
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isUnderAttack())
		{
			return true;
		}
	}
	return false;
}

bool Squad::needUnit(UnitType type)
{
	//1. Check if prio is set to Inactive squad.
	if (priority >= 1000)
	{	
		return false;
	}

	int noCreated = 1;
	if (BuildPlanner::isZerg())
	{
		if (type.isTwoUnitsInOneEgg())
		{
			noCreated = 2;
		}
	}
	
	//2. Check setup
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).equals(type))
		{
			//Found a matching setup, see if there is room
			if (setup.at(i).current + BuildPlanner::getInstance()->noInProduction(type) + noCreated <= setup.at(i).no)
			{
				return true;
			}
		}
	}

	return false;
}

vector<BaseAgent*> Squad::getMembers()
{
	return agents;
}

bool Squad::addMember(BaseAgent* agent)
{
	if (priority >= 1000)
	{
		//Check if prio is above Inactive squad.
		return false;
	}

	//Step 1. Check if the agent already is in the squad
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->getUnitID() == agent->getUnitID())
		{
			//Remove it, and add again to update the types.
			//Needed for morphing units like Lurkers.
			removeMember(agent);
		}
	}

	//Step 2. Check if we have room for this type of agent.
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).equals(agent->getUnitType()))
		{
			//Found a matching setup, see if there is room
			if (setup.at(i).current < setup.at(i).no)
			{
				//Yes we have, add it to the squad
				agents.push_back(agent);
				agent->setSquadID(id);
				setup.at(i).current++;
				
				if (goal.x() >= 0)
				{
					agent->setGoal(goal);
				}

				return true;
			}
		}
	}

	return false;
}

void Squad::printInfo()
{
	string f = "NotFull";
	if (isFull())
	{
		f = "Full";
	}
	string a = "Inactive";
	if (isActive())
	{
		a = "Active";
	}
	string m = "Ground";
	if (isAir())
	{
		m = "Air";
	}

	Broodwar->printf("[SQ %d-%s] %s (%s, %s) prio: %d units: %d", id, m.c_str(), name.c_str(), f.c_str(), a.c_str(), priority, getSize());
}

void Squad::printFullInfo()
{
	printInfo();

	for (int i = 0; i < (int)setup.size(); i++)
	{
		Broodwar->printf("%s, %d/%d", setup.at(i).type.getName().c_str(), setup.at(i).current, setup.at(i).no);
	}
}

bool Squad::isFull()
{
	//1. Check setup
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).current < setup.at(i).no)
		{
			return false;
		}
	}

	//2. Check that all units are alive and ready
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (!agents.at(i)->isAlive()) return false;
		if (agents.at(i)->getUnit()->isBeingConstructed()) return false;
	}

	//3. Check if some morphing is needed
	if (morphs.getID() != UnitTypes::Unknown.getID())
	{
		for (int i = 0; i < (int)agents.size(); i++)
		{
			if (morphs.getID() == UnitTypes::Zerg_Lurker.getID() && agents.at(i)->isOfType(UnitTypes::Zerg_Hydralisk))
			{
				return false;
			}
			if (morphs.getID() == UnitTypes::Zerg_Devourer.getID() && agents.at(i)->isOfType(UnitTypes::Zerg_Mutalisk))
			{
				return false;
			}
			if (morphs.getID() == UnitTypes::Zerg_Guardian.getID() && agents.at(i)->isOfType(UnitTypes::Zerg_Mutalisk))
			{
				return false;
			}
		}
	}

	return true;
}

void Squad::removeMember(BaseAgent* agent)
{
	//Step 1. Remove the agent instance
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->getUnitID() == agent->getUnitID())
		{
			agents.at(i)->setSquadID(-1);
			agents.at(i)->assignToDefend();
			agents.erase(agents.begin() + i);
			break;
		}
	}

	//Step 2. Update the setup list
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).equals(agent->getUnitType()))
		{
			setup.at(i).current--;
		}
	}

	//Step 3. If Explorer, set destination as explored (to avoid being killed at the same
	//place over and over again).
	if (isExplorer())
	{
		TilePosition goal = agent->getGoal();
		if (goal.x() >= 0)
		{
			ExplorationManager::getInstance()->setExplored(goal);
		}
	}
}

BaseAgent* Squad::removeMember(UnitType type)
{
	BaseAgent* agent = NULL;

	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (UnitSetup::equals(agents.at(i)->getUnitType(), type))
		{
			agent = agents.at(i);
			break;
		}
	}

	if (agent != NULL)
	{
		removeMember(agent);
	}

	return agent;
}

bool Squad::isGathered()
{
	//If we only have 1 unit in the squad...
	if (getSize() == 1)
	{
		if (isFull())
		{
			return true;
		}
	}

	if (getSize() > 0)
	{
		TilePosition center = getCenter();
		int tot = 0;
		int within = 0;
		int maxRange = 4*32;

		for (int i = 1; i < (int)agents.size(); i++)
		{
			if (agents.at(i)->isAlive())
			{
				double dist = agents.at(i)->getUnit()->getDistance(Position(center));
				if (dist <= maxRange)
				{
					within++;
				}
				tot++;
			}
		}

		int pct = within * 100 / tot;
		if (pct >= 90)
		{
			return true;
		}
	}

	return false;
}

void Squad::defend(TilePosition mGoal)
{
	if (mGoal.x() == -1 || mGoal.y() == -1) return;

	if (currentState != STATE_DEFEND)
	{
		if (currentState == STATE_ASSIST && !isUnderAttack())
		{
			currentState = STATE_DEFEND;
		}
	}
	setGoal(mGoal);
}

void Squad::attack(TilePosition mGoal)
{
	if (mGoal.x() == -1 || mGoal.y() == -1) return;
	
	if (currentState != STATE_ATTACK)
	{
		if (!isUnderAttack())
		{
			if (isActive())
			{
				currentState = STATE_ATTACK;
			}
		}
	}

	if (isActive())
	{
		setGoal(mGoal);
	}
}

void Squad::assist(TilePosition mGoal)
{
	if (mGoal.x() == -1 || mGoal.y() == -1) return;

	if (currentState != STATE_ASSIST)
	{
		if (!isUnderAttack() && currentState == STATE_DEFEND)
		{
			currentState = STATE_ASSIST;
			setGoal(mGoal);
		}
	}
	else
	{
		if (goal.x() == -1)
		{
			setGoal(mGoal);
		}
	}
}

void Squad::setGoal(TilePosition mGoal)
{
	if (isAttacking())
	{
		if (goal.x() != -1)
		{
			return;
		}
	}

	if (mGoal.x() != goal.x() || mGoal.y() != goal.y())
	{
		goalSetFrame = Broodwar->getFrameCount();
		if (isGround())
		{
			int d = (int)goal.getDistance(mGoal);
			if (d >= 10)
			{
				if ((int)agents.size() > 0)
				{
					Pathfinder::getInstance()->requestPath(getCenter(), mGoal);
					if (!Pathfinder::getInstance()->isReady(getCenter(), mGoal))
					{
						return;
					}
					path = Pathfinder::getInstance()->getPath(getCenter(), mGoal);
					arrivedFrame = -1;
					pathIndex = 10;

					if (path.size() == 0) return;
				}
			}
		}

		this->goal = mGoal;
		setMemberGoals(goal);
	}
}

TilePosition Squad::nextMovePosition()
{
	if (path.size() <= 0)
	{
		return goal;
	}
	if (isAir())
	{
		return goal;
	}

	if (pathIndex >= (int)path.size())
	{
		return goal;
	}

	if (arrivedFrame == -1)
	{
		for (int i = 0; i < (int)agents.size(); i++)
		{
			int seekDist = agents.at(i)->getUnitType().seekRange();
			int dist = (int)agents.at(i)->getUnit()->getPosition().getDistance(Position(path.at(pathIndex)));
			if (dist <= seekDist)
			{
				arrivedFrame = Broodwar->getFrameCount();
				break;
			}
		}
	}

	if (arrivedFrame != -1)
	{
		int cFrame = Broodwar->getFrameCount();
		if (cFrame - arrivedFrame >= 200)
		{
			pathIndex += 10;
			if (pathIndex >= (int)path.size())
			{
				pathIndex = (int)path.size() - 1;
			}
			arrivedFrame = -1;
		}
	}

	TilePosition cGoal = path.at(pathIndex);
	setMemberGoals(cGoal);

	return cGoal;
}

void Squad::clearGoal()
{
	this->goal = TilePosition(-1, -1);
	setMemberGoals(goal);
}

void Squad::setMemberGoals(TilePosition cGoal)
{
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			agents.at(i)->setGoal(cGoal);
		}
	}
}

TilePosition Squad::getGoal()
{
	return goal;
}

bool Squad::hasGoal()
{
	int elapsed = Broodwar->getFrameCount() - goalSetFrame;
	if (elapsed >= 600)
	{
		if (!isAttacking())
		{
			goal = TilePosition(-1, -1);
		}
	}
	
	if (goal.x() < 0 || goal.y() < 0)
	{
		return false;
	}
	return true;
}

bool Squad::isThisGoal(TilePosition mGoal)
{
	int xDiff = goal.x() - mGoal.x();
	if (xDiff < 0) xDiff *= -1;
	int yDiff = goal.y() - mGoal.y();
	if (yDiff < 0) yDiff *= -1;

	//Broodwar->printf("SQ %d: (%d,%d) = (%d,%d)", id, goal.x(), goal.y(), mGoal.x(), mGoal.y());
	if (xDiff <= 2 && yDiff <= 2)
	{
		return true;
	}
	return false;
}

bool Squad::isCloseTo(TilePosition mGoal)
{
	double dist = mGoal.getDistance(goal);
	if (dist <= 3)
	{
		return true;
	}
	return false;
}

BaseAgent* Squad::getCenterAgent()
{
	TilePosition center = getCenter();

	BaseAgent* cAgent = NULL;
	double dist = 10000;

	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if ( (isAir() && agents.at(i)->getUnitType().isFlyer()) || (isGround() && !agents.at(i)->getUnitType().isFlyer()))
			{
				double cDist = agents.at(i)->getUnit()->getTilePosition().getDistance(center);
				if (cDist < dist)
				{
					cAgent = agents.at(i);
					dist = cDist;
				}
			}
		}
	}

	return cAgent;
}

TilePosition Squad::getCenter()
{
	if (agents.size() == 1)
	{
		return agents.at(0)->getUnit()->getTilePosition();
	}

	int cX = 0;
	int cY = 0;
	int cnt = 0;

	//Calculate sum (x,y)
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			cX += agents.at(i)->getUnit()->getTilePosition().x();
			cY += agents.at(i)->getUnit()->getTilePosition().y();
			cnt++;
		}
	}

	//Calculate average (x,y)
	if(cnt > 0)
	{
		cX = cX / cnt;
		cY = cY / cnt;
	}

	//To make sure the center is in a walkable tile, we need to
	//find the unit closest to center
	TilePosition c = TilePosition(cX, cY);
	TilePosition bestSpot = c;
	double bestDist = 10000;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			if ( (isAir() && agents.at(i)->getUnitType().isFlyer()) || (isGround() && !agents.at(i)->getUnitType().isFlyer()))
			{
				double dist = agents.at(i)->getUnit()->getTilePosition().getDistance(c);
				if (dist < bestDist)
				{
					bestDist = dist;
					bestSpot = agents.at(i)->getUnit()->getTilePosition();
				}
			}
		}
	}

	return bestSpot;
}

int Squad::getSize()
{
	int no = 0;
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive() && !agents.at(i)->getUnit()->isBeingConstructed())
		{
			no++;
		}
	}
	return no;
}

int Squad::getTotalUnits()
{
	int tot = 0;

	for (int i = 0; i < (int)setup.size(); i++)
	{
		tot += setup.at(i).no;
	}

	return tot;
}

int Squad::getStrength()
{
	int str = 0;

	for (int i = 1; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			str += agents.at(i)->getUnitType().destroyScore();
		}
	}

	return str;
}

bool Squad::isOffensive()
{
	return type == OFFENSIVE;
}

bool Squad::isDefensive()
{
	return type == DEFENSIVE;
}

bool Squad::isExplorer()
{
	return type == EXPLORER;
}

bool Squad::isSupport()
{
	return type == SUPPORT;
}

bool Squad::isBunkerDefend()
{
	return type == BUNKER;
}

bool Squad::isSpecial()
{
	return type == SPECIAL;
}

bool Squad::isGround()
{
	return moveType == GROUND;
}

bool Squad::isAir()
{
	return moveType == AIR;
}

void Squad::disband(TilePosition retreatPoint)
{
	for (int i = 1; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			agents.at(i)->setGoal(retreatPoint);
		}
	}
	active = false;
}

int Squad::getHealthPct()
{
	int need = 0;
	int current = 0;

	for (int i = 0; i < (int)setup.size(); i++)
	{
		need += setup.at(i).no;
		current += setup.at(i).current;
	}

	return (int)(current * 100 / need);
}

bool Squad::canMerge(Squad* squad)
{
	//Step 1. Check so the two squads are different
	if (squad->getID() == getID())
	{
		return false;
	}

	//Step 2. If the other squad is full, we dont
	//use it to merge with.
	if (squad->isFull())
	{
		return false;
	}

	if (getHealthPct() < squad->getHealthPct())
	{
		//Can only merge with smaller squads
		return false;
	}

	//Step 3. Check if we can merge
	bool ok = false;
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).current < setup.at(i).no)
		{
			//I need this number of units of the specified type
			int need = setup.at(i).no - setup.at(i).current;
			if (squad->hasUnits(setup.at(i).type, need))
			{
				//The other squad have something we need, merge
				ok = true;
			}
		}
	}
	if (!ok)
	{
		return false;
	}

	//Step 4. Do the merge
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).current < setup.at(i).no)
		{
			int need = setup.at(i).no - setup.at(i).current;
			for (int j = 0; j < need; j++)
			{
				BaseAgent* agent = squad->removeMember(setup.at(i).type);
				if (agent != NULL)
				{
					if (!agent->isAlive() || !agent->getUnit()->exists())
				{
						Broodwar->printf("[%d] WARNING merge with destroyed agent %s", id, agent->getUnitType().getName().c_str());
					}
					addMember(agent);
				}
			}
		}
	}

	return true;
}

bool Squad::hasUnits(UnitType type, int no)
{
	for (int i = 0; i < (int)setup.size(); i++)
	{
		if (setup.at(i).equals(type))
		{
			if (setup.at(i).current >= no)
			{
				//I have these units
				return true;
			}
		}
	}
	return false;
}

bool Squad::contains(UnitType type)
{
	for (int i = 0; i < (int)agents.size();i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive() && agent->isOfType(type))
		{
			return true;
		}
	}
	return false;
}

TilePosition Squad::getClosestStartLocation(TilePosition pos)
{
	TilePosition sloc = pos;
	double bestDist = 10000;

	for(set<BaseLocation*>::const_iterator i=getStartLocations().begin();i!=getStartLocations().end();i++)
	{
		TilePosition basePos = (*i)->getTilePosition();
		double dist = pos.getDistance(basePos);
		if (dist < bestDist)
		{
			bestDist = dist;
			sloc = basePos;
		}
	}

	return sloc;
}

TilePosition Squad::getNextStartLocation()
{
	for(set<BaseLocation*>::const_iterator i=getStartLocations().begin();i!=getStartLocations().end();i++)
	{
		TilePosition basePos = (*i)->getTilePosition();
		if (!isVisible(basePos))
		{
			return basePos;
		}
		else
		{
			if ((int)agents.size() > 0)
			{
				UnitAgent* uagent = (UnitAgent*)agents.at(0);
				int eCnt = uagent->enemyUnitsWithinRange(10 * 32);
				if (eCnt > 0)
				{
					return TilePosition(-1, -1);
				}
			}

			hasVisited.push_back(basePos);
		}
	}
	return TilePosition(-1, -1);
}

bool Squad::isVisible(TilePosition pos)
{
	if (!ExplorationManager::canReach(Broodwar->self()->getStartLocation(), pos))
	{
		return true;
	}

	if (Broodwar->isVisible(pos))
	{
		return true;
	}

	if (getCenter().getDistance(pos) <= 3)
	{
		return true;
	}

	for (int i = 0; i < (int)hasVisited.size(); i++)
	{
		TilePosition vPos = hasVisited.at(i);
		if (vPos.x() == pos.x() && vPos.y() == pos.y())
		{
			return true;
		}
	}

	return false;
}
