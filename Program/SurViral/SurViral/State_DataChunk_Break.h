/*
	Filename: State_DataChunk_Break.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for BREAK (DataChunk State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#ifndef _STATE_DATACHUNK_BREAK
#define _STATE_DATACHUNK_BREAK

//Library dependecies

//Class dependencies
#include "IAIState.h"
#include "AIAgent.h"
#include "Shared_Resources.h"

//Forward declarations
class b2Body;
class StateMachine_DataChunk;

class State_DataChunk_Break : public IAIState<AIAgent>
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	State_DataChunk_Break(AIAgent* actor,PhysicsManagerPointer physicsmgr):
    IAIState(actor),
	mPhysicsMgr(physicsmgr),
	mCounter(1000.0f),
	mSM(NULL)
	{
		_init();
	}
	virtual ~State_DataChunk_Break()
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
	float mCounter;

	StateMachine_DataChunk* mSM; 
	//----- INTERNAL FUNCTIONS -----
	void _init();

};

#endif