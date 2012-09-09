/*
	Filename: StateMachine_CriticalSection.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Base class for states machines of AI agents
	Comments: Implementation of state machine class for a Critical Section (SurViral game)
	Attribution: It is loosely based in two books implementation of a flexible state machine:
				 "Game coding complete"  http://www.mcshaffry.com/GameCode/
				 "Programming Game AI by Example" http://www.ai-junkie.com/books/toc_pgaibe.html
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "StateMachine_CriticalSection.h"
#include "GameEvents.h"

//Events plugin
bool StateMachine_CriticalSection::HandleEvent(const EventData& data)			
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
bool StateMachine_CriticalSection::HandleCollision(const CollisionEventData& data) 
{
	bool eventprocessed(false);

	const ContactInfo collisiondata (data.GetCollisionData());

	//IF - New collision event
	if(data.GetEventType() == Event_NewCollision)
	{
		if(collisiondata.agent1->GetType() == PLAYER
			||
			collisiondata.agent2->GetType() == PLAYER)
		{
			++mPlayerContacts;
		}
	}//ELSE - Collision deleted event
	if(data.GetEventType() == Event_DeletedCollision)
	{
		if(collisiondata.agent1->GetType() == PLAYER
			||
			collisiondata.agent2->GetType() == PLAYER)
		{
			--mPlayerContacts;
		}
	}

	//Handle current state collision too
	if(mCurrentState)
		return (mCurrentState->HandleCollision(data) || eventprocessed);	
	else 
		return eventprocessed;
}


void StateMachine_CriticalSection::_createState(const std::string& statename)  //Creates a state given a state name (string) - it is specific of every state machine
{
	if(statename == "Wait")
	{
		mCurrentState = new State_CriticalSection_Wait(mActor,mPhysicsMgr);
	}
	else if(statename == "Corrupted")
	{
		mCurrentState = new State_CriticalSection_Corrupted(mActor,mPhysicsMgr);
	}
	//ELSE - Not defined state!
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("StateMachine_CriticalSection::_createState","State name '" + statename + "' for a MadMonkey is not correct! Program BUG!",LOGEXCEPTION);
	}//IF

	//IF - Created
	if(mCurrentState)
	{
		//Memo state name and execute enter action
		mCurrentStateName = statename;
		mCurrentState->EnterAction();
	}//IF
}