/*
	Filename: State_DataChunk_Infect.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for INFECT (DataChunk State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#include "State_DataChunk_Infect.h"
#include "StateMachine_DataChunk.h"
#include "IndieLibManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"

//Update ticks for state
void State_DataChunk_Infect::Update(float dt)
{
	mCounter -=dt;
	if(mCounter <= 0.0f)
		mActor->Destroy();
}

//Enter Action for state
void State_DataChunk_Infect::EnterAction()
{
	//Set color to show it is infected!
	mActor->GetGFXSprite()->SetFade(255,0,0,0);

}

//Exit Action for state
void State_DataChunk_Infect::ExitAction()
{
	//Send event to notify infection!
	DataBlockInfectedInfo eventinfo(mActor->GetPosition(),false);
	SingletonGameEventMgr::Instance()->QueueEvent(
									EventDataPointer(new DataBlockInfectedEvent(Event_DataBlockInfected,eventinfo))
									);
}

//Events plugin
bool State_DataChunk_Infect::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

//Physics collisions plugin
bool State_DataChunk_Infect::HandleCollision(const CollisionEventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

void State_DataChunk_Infect::_init()
{

}