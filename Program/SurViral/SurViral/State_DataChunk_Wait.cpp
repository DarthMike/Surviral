/*
	Filename: State_DataChunk_Wait.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for WAIT (DataChunk State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#include "State_DataChunk_Wait.h"
#include "StateMachine_DataChunk.h"


//Update ticks for state
void State_DataChunk_Wait::Update(float)
{
	//It waits till it is moved
	Vector2 posdifference (mInitialPos - mActor->GetPosition());
	
	if(posdifference.Length() >= mMovingDist)
	{
		mActor->GetStateMachine()->SetState("Break");
	}
}

//Enter Action for state
void State_DataChunk_Wait::EnterAction()
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
	mMovingDist *= 0.8f;	
}

//Exit Action for state
void State_DataChunk_Wait::ExitAction()
{

}

//Events plugin
bool State_DataChunk_Wait::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

//Physics collisions plugin
bool State_DataChunk_Wait::HandleCollision(const CollisionEventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

void State_DataChunk_Wait::_init()
{
}