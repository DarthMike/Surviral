/*
	Filename: StateMachine_SpawnVirus.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Base class for states machines of AI agents
	Comments: Implementation of state machine class for a Spawned Virus (SurViral game)
	Attribution: It is loosely based in two books implementation of a flexible state machine:
				 "Game coding complete"  http://www.mcshaffry.com/GameCode/
				 "Programming Game AI by Example" http://www.ai-junkie.com/books/toc_pgaibe.html
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "StateMachine_SpawnVirus.h"
#include "GameEvents.h"

//Events plugin
bool StateMachine_SpawnVirus::HandleEvent(const EventData& data)			
{
	bool eventprocessed(false);

	//IF - Exists state
	if(mCurrentState)
	{
		//Process overall state change
		//For now nothing
		
		//Forward event to current state
		return(eventprocessed || mCurrentState->HandleEvent(data));
	}

	//No state defined
	return eventprocessed;
}

//Physics collisions events
bool StateMachine_SpawnVirus::HandleCollision(const CollisionEventData& data) 
{
	//Use default implementation
	const ContactInfo collisiondata (data.GetCollisionData());

	//IF - New collision event
	if(data.GetEventType() == Event_NewCollision)
	{
		++mContacts;
	}//ELSE - Collision deleted event
	if(data.GetEventType() == Event_DeletedCollision)
	{
		--mContacts;
	}

	return (StateMachine<AIAgent>::HandleCollision(data));	
}


void StateMachine_SpawnVirus::_createState(const std::string& statename)  //Creates a state given a state name (string) - it is specific of every state machine
{
	if(statename == "Move")
	{
		mCurrentState = new State_SpawnVirus_Move(mActor,mPhysicsMgr);
	}
	//ELSE - Not defined state!
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("StateMachine_SpawnVirus::_createState","State name '" + statename + "' for a MadMonkey is not correct! Program BUG!",LOGEXCEPTION);
	}//IF

	//IF - Created
	if(mCurrentState)
	{
		//Memo state name and execute enter action
		mCurrentStateName = statename;
		mCurrentState->EnterAction();
	}//IF
}