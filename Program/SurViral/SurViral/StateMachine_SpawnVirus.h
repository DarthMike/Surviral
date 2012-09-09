/*
	Filename: StateMachine_SpawnVirus.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Base class for states machines of AI agents
	Comments: Implementation of state machine class for a Spawned Virus (SurViral game)
	Attribution: It is loosely based in two books implementation of a flexible state machine:
				 "Game coding complete"  http://www.mcshaffry.com/GameCode/
				 "Programming Game AI by Example" http://www.ai-junkie.com/books/toc_pgaibe.html
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _SPAWNEDVIRUSSTATEMACHINE
#define _SPAWNEDVIRUSSTATEMACHINE

//Library dependencies

//Class dependencies
#include "StateMachine.h"
#include "SM_SpawnVirus_States.h"
#include "AIAgent.h"
#include "Shared_Resources.h"
#include "LogManager.h"

class StateMachine_SpawnVirus : public StateMachine<AIAgent>
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	StateMachine_SpawnVirus(AIAgent* actor, PhysicsManagerPointer physicsptr, const std::string& type):
	  StateMachine(actor,physicsptr,type),
	  mPhysicsMgr(physicsptr),
	  mContacts(0)
	{
		assert(mSMType == "SpawnVirus");
		//On start default to state "Move"
		SetState("Move");
	}
	virtual ~StateMachine_SpawnVirus()
	{
		if(mCurrentState)
		{
			delete mCurrentState;
			mCurrentState = NULL;
		}
	}
	//----- GET/SET FUNCTIONS -----

	//----- OTHER FUNCTIONS -----
	//Implementation of virtual interface
	virtual bool HandleEvent(const EventData& data);				//Process possible events (with default implementation)
	virtual bool HandleCollision(const CollisionEventData& data); //Process possible collisions (with default implementation)
	//----- PUBLIC VARIABLES ------
private:
	//----- INTERNAL VARIABLES -----
	PhysicsManagerPointer mPhysicsMgr;

	int mContacts;
	//----- INTERNAL FUNCTIONS -----
	//Implementation of "CreateState" Method
	virtual void _createState(const std::string& statename);  //Creates a state given a state name (string) - it is specific of every state machine
};

#endif
