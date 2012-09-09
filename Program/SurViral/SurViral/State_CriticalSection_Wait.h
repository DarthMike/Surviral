/*
	Filename: State_CriticalSection_Wait.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for WAIT (CriticalSection State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#ifndef _STATE_CRITICALSECTION_WAIT
#define _STATE_CRITICALSECTION_WAIT

//Library dependecies

//Class dependencies
#include "IAIState.h"
#include "AIAgent.h"
#include "Shared_Resources.h"

class State_CriticalSection_Wait : public IAIState<AIAgent>
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	State_CriticalSection_Wait(AIAgent* actor,PhysicsManagerPointer physicsmgr):
    IAIState(actor),
	mPhysicsMgr(physicsmgr),
	mActorBody(NULL),
	mMovingDist(1.0f)
	{
		_init();
	}
	virtual ~State_CriticalSection_Wait()
	{}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Interface implementation
	virtual void Update(float dt);   
	virtual void EnterAction();
	virtual void ExitAction();
	virtual bool HandleEvent(const EventData& data);
	virtual bool HandleCollision(const CollisionEventData& data); 
	//----- PUBLIC VARIABLES ------
private:
	//----- INTERNAL VARIABLES -----
	PhysicsManagerPointer mPhysicsMgr;
	Vector2 mInitialPos;
	float mMovingDist;
	b2Body* mActorBody;

	//----- INTERNAL FUNCTIONS -----
	void _init();

};

#endif