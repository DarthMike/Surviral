/*
	Filename: State_DataChunk_Break.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for BREAK (DataChunk State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#include "State_DataChunk_Break.h"
#include "StateMachine_DataChunk.h"
#include "IndieLibManager.h"
#include "GameEvents.h"
#include "GameEventManager.h"


//Update ticks for state
void State_DataChunk_Break::Update(float dt)
{
	//Waits to be touched by virus to be infected

	if(mSM->GetPlayerContacts() > 0)
		mCounter-=dt;
	else
		mCounter = 500.0f;

	if(mCounter <= 0.0f)
	{
		mActor->GetStateMachine()->SetState("Infect");
	}
}

//Enter Action for state
void State_DataChunk_Break::EnterAction()
{
	//Set color to show it is moved!
	mActor->GetGFXSprite()->SetFade(255,255,255,128);

	//Send event to notify
	DataBlockCorruptedInfo eventinfo(mActor->GetPosition(),false);
	SingletonGameEventMgr::Instance()->QueueEvent(
									EventDataPointer(new DataBlockCorruptedEvent(Event_DataBlockCorrupted,eventinfo))
									);
}

//Exit Action for state
void State_DataChunk_Break::ExitAction()
{
	
}

//Events plugin
bool State_DataChunk_Break::HandleEvent(const EventData&)
{
	bool eventprocessed(false);
	
	return eventprocessed;
}

//Physics collisions plugin
bool State_DataChunk_Break::HandleCollision(const CollisionEventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

void State_DataChunk_Break::_init()
{
	mSM = static_cast<StateMachine_DataChunk*>(mActor->GetStateMachine());
}