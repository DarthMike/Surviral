/*
	Filename: State_CriticalSection_Wait.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for WAIT (CriticalSection State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/
#include "State_CriticalSection_Wait.h"
#include "StateMachine_CriticalSection.h"
#include "GameEvents.h"
#include "GameEventManager.h"

//Update ticks for state
void State_CriticalSection_Wait::Update(float)
{
	//It waits till it is moved
	Vector2 posdifference (mInitialPos - mActor->GetPosition());
	
	if(posdifference.Length() >= mMovingDist)
	{
		mActor->GetStateMachine()->SetState("Corrupted");
	}
}

//Enter Action for state
void State_CriticalSection_Wait::EnterAction()
{
	//Calculate needed values
	mInitialPos = mActor->GetPosition();
	mActorBody = mActor->GetPhysicalBody();
	b2Shape* actorshape (mActorBody->GetShapeList());

	b2AABB shapeaabb;
	b2XForm aabbXform;
	aabbXform.SetIdentity();

	actorshape->ComputeAABB(&shapeaabb,aabbXform);

	float xaxisdist (shapeaabb.upperBound.x - shapeaabb.lowerBound.x);
	float yaxisdist (shapeaabb.upperBound.y - shapeaabb.lowerBound.y);

	mMovingDist = (yaxisdist <= xaxisdist) ? yaxisdist : xaxisdist;	
}

//Exit Action for state
void State_CriticalSection_Wait::ExitAction()
{

}

//Events plugin
bool State_CriticalSection_Wait::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

//Physics collisions plugin
bool State_CriticalSection_Wait::HandleCollision(const CollisionEventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

void State_CriticalSection_Wait::_init()
{
}