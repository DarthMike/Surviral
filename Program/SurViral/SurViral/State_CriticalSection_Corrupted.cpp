/*
	Filename: State_CriticalSection_Corrupted.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for CORRUPTED (CriticalSection State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/


#include "State_CriticalSection_Corrupted.h"
#include "StateMachine_CriticalSection.h"
#include "IndieLibManager.h"
#include "GameEvents.h"
#include "GameEventManager.h"

//Update ticks for state
void State_CriticalSection_Corrupted::Update(float)
{

}

//Enter Action for state
void State_CriticalSection_Corrupted::EnterAction()
{
	//Modify color of data block to show it is corrupted
	mActor->GetGFXSprite()->SetTint(255,0,0);
	//Send DataBlock Corrupted event
	DataBlockCorruptedInfo eventinfo(mActor->GetPosition(),true);
	SingletonGameEventMgr::Instance()->QueueEvent(
			EventDataPointer(new DataBlockCorruptedEvent(Event_DataBlockCorrupted,eventinfo))
			);

}

//Exit Action for state
void State_CriticalSection_Corrupted::ExitAction()
{

}

//Events plugin
bool State_CriticalSection_Corrupted::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

//Physics collisions plugin
bool State_CriticalSection_Corrupted::HandleCollision(const CollisionEventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

void State_CriticalSection_Corrupted::_init()
{
}