/*
	Filename: State_SpawnVirus_Move.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for MOVE (SpawnVirus State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#include "State_SpawnVirus_Move.h"
#include "StateMachine_SpawnVirus.h"
#include "IndieLibManager.h"
#include "PhysicsManager.h"
#include "Math.h"

//Update ticks for state
void State_SpawnVirus_Move::Update(float)
{
	//Update position of physics body with movement from UI
	if(mPhysicsMgr->IsPhysicsStepped())
	{
		b2Vec2 movspeed (static_cast<float>(mMovingDir.x),
			             static_cast<float>(mMovingDir.y));

		//Scale move direction by speed to move
		movspeed *= 8.0f;   ///HACK/// SHOULD READ FROM SOME FILE

		mAgentBody->SetLinearVelocity(movspeed);
		mAgentBody->WakeUp();
	}
}

//Enter Action for state
void State_SpawnVirus_Move::EnterAction()
{
	//Calculate initial values
	mMovingDir.x = static_cast<double>((SingletonMath::Instance()->NewRandom(0,100)));
	mMovingDir.y = static_cast<double>((SingletonMath::Instance()->NewRandom(0,100)));
	mMovingDir.Normalise();
	mAgentBody = mActor->GetPhysicalBody();
}

//Exit Action for state
void State_SpawnVirus_Move::ExitAction()
{
}

//Events plugin
bool State_SpawnVirus_Move::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

//Physics collisions plugin
bool State_SpawnVirus_Move::HandleCollision(const CollisionEventData& eventdata)
{
	bool eventprocessed(false);

	const ContactInfo collisiondata (eventdata.GetCollisionData());

	if(eventdata.GetEventType() == Event_NewCollision)
	{
		//Get the contact normal
		b2Vec2 contactnormal(collisiondata.normal);
		contactnormal.Normalize();

		//IF - This body is "1" in collision
		if(collisiondata.collidedbody2 == mAgentBody)
		{
			//invert direction of normal
			contactnormal = -contactnormal;
		}
	
		float anglepercent (static_cast<float>(SingletonMath::Instance()->NewRandom(0,100)) / 100);

		b2XForm rotationxform;
		rotationxform.R.Set(anglepercent * SingletonMath::Instance()->AngleToRadians(30.0f));
		contactnormal = b2Mul(rotationxform,contactnormal);
		mMovingDir.x = contactnormal.x;
		mMovingDir.y = contactnormal.y;
		mMovingDir.Normalise();
	}

	return eventprocessed;
}

void State_SpawnVirus_Move::_init()
{

}