#include "PFManager.h"
#include "PFFunctions.h"
#include "UnitAgent.h"
#include "AgentManager.h"
#include "ExplorationManager.h"
#include "Commander.h"
#include "Squad.h"
#include "CoverMap.h"
#include "Profiler.h"

bool PFManager::instanceFlag = false;
PFManager* PFManager::instance = NULL;

PFManager::PFManager()
{
	checkRange = 48;
	stepSize = 16;
	mapW = Broodwar->mapWidth() * 32;
	mapH = Broodwar->mapHeight() * 32;
}

PFManager::~PFManager()
{
	instanceFlag = false;
	delete instance;
}

PFManager* PFManager::getInstance()
{
	if (!instanceFlag)
	{
		instance = new PFManager();
		instanceFlag = true;
	}
	return instance;
}

void PFManager::computeAttackingUnitActions(BaseAgent* agent, TilePosition goal, bool defensive)
{
	computeAttackingUnitActions(agent, goal, defensive, false);
}

void PFManager::computeAttackingUnitActions(BaseAgent* agent, TilePosition goal, bool defensive, bool forceMove)
{
	Unit* unit = agent->getUnit();

	if (!defensive || !forceMove)
	{
		//if (agent->getUnit()->isStartingAttack()) return;
		if (agent->getUnit()->isAttacking()) return;
		if (agent->getUnit()->isSieged()) return;
	}
	if (agent->getUnit()->isLoaded()) return;

	//PF
	int unitX = unit->getPosition().x();
	int unitY = unit->getPosition().y();

	float bestP = getAttackingUnitP(agent, unitX, unitY, defensive);
	float cP = 0;
	
	float startP = bestP;
    int bestX = -1;
    int bestY = -1;

	for (int cX = unitX - checkRange; cX <= unitX + checkRange; cX += stepSize)
	{
        for (int cY = unitY - checkRange; cY <= unitY + checkRange; cY += stepSize)
		{
            if (cX >= 0 && cY >= 0 && cX <= mapW && cY <= mapH)
			{
				cP = getAttackingUnitP(agent, cX, cY, defensive);
				//Broodwar->printf("p(%d,%d)=%d",cX,cY,cP);

                if (cP != bestP)
				{
					bestP = cP;
                    bestX = cX;
                    bestY = cY;
                }
            }
        }
    }
	
	if (bestX >= 0 && bestY >= 0)
	{
		Position toMove(bestX, bestY);
		if (!defensive)
		{
			unit->attack(toMove);
		}
		else
		{
			unit->rightClick(toMove);
		}
		return;
    }
	//EndPF

	TilePosition checkpoint = goal;
	if (agent->getSquadID() >= 0)
	{
		Squad* sq = Commander::getInstance()->getSquad(agent->getSquadID());
		if (sq != NULL)
		{
			checkpoint = sq->nextMovePosition();
		}
	}
	
	if (goal.x() != -1)
	{
		moveToGoal(agent, checkpoint, goal);
	}
}

bool PFManager::moveToGoal(BaseAgent* agent,  TilePosition checkpoint, TilePosition goal)
{
	if (checkpoint.x() == -1 || goal.x() == -1) return false;
	Unit* unit = agent->getUnit();

	if (unit->isStartingAttack() || unit->isAttacking())
	{
		return false;
	}

	Position toReach = Position(checkpoint);
	double distToReach = toReach.getDistance(unit->getPosition());

	int engageDist = unit->getType().groundWeapon().maxRange();
	if (agent->isOfType(UnitTypes::Terran_Medic))
	{
		engageDist = 6 * 32;
	}
	if (engageDist <= 64)
	{
		engageDist = 64;
	}
	
	if (distToReach <= engageDist)
	{
		if (unit->isMoving()) unit->stop();
		return true;
	}
	
	//Move
	if (!unit->isMoving()) return unit->attack(toReach);
	else return true;
}

float PFManager::getAttackingUnitP(BaseAgent* agent, int cX, int cY, bool defensive)
{
	float p = 0;
	Position pos(cX, cY);

	//Enemy Units
	int eCnt = 0;
	float p_plus = 0;
	float p_minus = 0;
	for(set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++)
	{
		//Enemy seen
		float dist = PFFunctions::getDistance(pos, (*i));
		float ptmp = PFFunctions::calcAttackingUnitP(dist, agent->getUnit(), (*i), defensive);

		if (ptmp > 0)
		{
			if (ptmp > p_plus)
			{
				p_plus = ptmp;
			}
		}
		if (ptmp < 0)
		{
			if (ptmp < p_minus)
			{
				p_minus = ptmp;
			}
		}
	}
	if (p_minus < 0)
	{
		p = p_minus;
	}
	else
	{
		p = p_plus;
	}

	//No enemy units found, use pathfinding
	if (p == 0)
	{
		return p;
	}
	
	//Own Units
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		if (agents.at(i)->isAlive())
		{
			float dist = PFFunctions::getDistance(pos, agents.at(i)->getUnit());
			float ptmp = PFFunctions::calcOwnUnitP(dist, agent->getUnit(), agents.at(i)->getUnit());

			p += ptmp;
		}
	}

	//Neutral Units
	for(set<Unit*>::const_iterator i=Broodwar->getNeutralUnits().begin();i!=Broodwar->getNeutralUnits().end();i++)
	{
		float dist = PFFunctions::getDistance(pos, (*i));
		float ptmp = PFFunctions::calcMineP(dist, agent->getUnit());

        p += ptmp;
	}

	//Broodwar->printf("[%d] o=%d", agent->getUnitID(), (int)p);

	//Terrain
	if (!Broodwar->isWalkable(cX/8, cY/8))
	{
		p -= 50;
	}

	//Broodwar->printf("[%d] t=%d", agent->getUnitID(), (int)p);

	return p;
}
