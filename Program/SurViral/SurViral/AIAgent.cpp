/*
	Filename: AIAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Agent controlled by AI
	Comments: Internally, it holds a state machine, and a solid body agent
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "AIAgent.h"
#include "PhysicsEvents.h"
#include "GameEvents.h"
#include "Creatable_StateMachines.h"
#include "PhysicsManager.h"
#include "IndieLibManager.h"

//Update object status
void AIAgent::UpdateState(float dt)
{
	if(!mActive)
		return;

	//Update GFX
	b2Vec2 position (mParams.physicbody->GetPosition());
	mParams.position = Vector2(position.x,position.y);
	mParams.rotation = mParams.physicbody->GetAngle();
	Vector2 pixposition = SingletonIndieLib::Instance()->FromCoordToPix(mParams.position);
	int posz(mParams.sprite.gfxentity->GetPosZ());
	mParams.sprite.gfxentity->SetPosition(static_cast<float>(pixposition.x),static_cast<float>(pixposition.y),posz);
	mParams.sprite.gfxentity->SetAngleXYZ(0.0f,0.0f,SingletonMath::Instance()->RadiansToAngle<float>(mParams.rotation,true));
	//Update animation 
	if(mAnimController)
	{
		mAnimController->Update(dt);
	}

	//Update state machine
	if(mStateMachine)
	{
		//IF - State change requested
		if(mStateChange && mNextStateRequested != "")
		{
			mStateMachine->SetState(mNextStateRequested);
			mStateChange = false;
		}//ELSE - No state change
		else
		{
			mStateMachine->Update(dt);
		}//IF
	}

	if(mOutOfLimits)
	{
		//Just Destroy agent
		Destroy(); 

		//Send event of AI out of Limits
		AgentOutInfo data(mId,mParams.GetType(),mParams.position,mParams.agentAI);
		//Send event as ai is out of limits
		SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new AgentOutEvent(Event_AgentOut,data))
											);
	}
	else
	{
		//Send event to update position
		AgentPositionInfo data(mId,mParams.GetType(),mParams.position,mParams.agentAI);
		//Send event as ai has new position
		SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new AgentOutEvent(Event_AgentPosition,data))
											);
	}
}

//Process possible collisions
bool AIAgent::HandleCollision(const CollisionEventData& data)
{
	//Forward collision to state machine
	if(mStateMachine)	
		mStateMachine->HandleCollision(data);
	
	return (true);
}

//Handle possible events
bool AIAgent::HandleEvent(const EventData& data)
{
	bool eventprocessed (false);
	
	//Forward event processing to state machine
	if(mStateMachine)
		eventprocessed = mStateMachine->HandleEvent(data);

	return eventprocessed;
}

//Handle out of limits
void AIAgent::HandleOutOfLimits(const OutOfLimitsEventData&) 
{ 
	mOutOfLimits = true;
}	

//Create from params
void AIAgent::Create( const GameAgentPar *params)	
{
	
	//Agent params should be of type
	//Copy parameters from creation 
	assert(params->GetType() == AI);
	
	//Cast down params hierarchy to this type of params
	const AIAgentPar *aiparams = static_cast<const AIAgentPar*>(params);
	//Copy all parameters
	mParams = *aiparams;
	
	//IF - Exists body, link all shapes to this agent
	if(mParams.physicbody)
	{
		mParams.physicbody->SetUserData(this);
		b2Shape* bodyshape (mParams.physicbody->GetShapeList());
		bodyshape->SetUserData(this);
		//LOOP - Bind all shapes to this agent
		while(bodyshape->GetNext())
		{
			bodyshape = bodyshape->GetNext();
			bodyshape->SetUserData(this);
		}//LOOP END
	}//IF

	//Syncronize body position for first time
	b2Vec2 position (mParams.physicbody->GetPosition());
	mParams.position = Vector2(position.x,position.y);

	//Parameters copied, create state machine
	_createStateMachine(mParams.agentAI);

	//Add animation sprite to agent
	if(mParams.sprite.gfxentity->GetAnimation() != NULL)
	{
		mAnimController = AnimationControllerPointer(new AnimationController(mParams.sprite.gfxentity,0,true,false));
	}

	//Finally, update internal tracking
	mActive = true;
}

//Destroy agent
void AIAgent::Destroy()
{
	if(!mActive)
		return;

	//Stop state machine
	if(mStateMachine)
	{
		mStateMachine->Stop();
	}

	//Delete from GFX system created sprite
	SingletonIndieLib::Instance()->Entity2dManager->Delete(mParams.sprite.gfxentity.get());

	//Delete bodies data
	std::string bodyname (mPhysicsMgr->GetBodyName(mParams.physicbody));
	mPhysicsMgr->DestroyBody(bodyname);

	//Finally, update internal tracking
	mActive = false;
}

//State machine state change
void AIAgent::ChangeState(const std::string& newstatename)
{
	if(!mActive)
		return;
	//Memorize
	mNextStateRequested = newstatename;			
	mStateChange = true;
}


//State machine class instantiation
void AIAgent::_createStateMachine( const std::string& SMName)
{
	//Association of text property and AI state machine instantiation
	if(SMName == "DataChunk")
	{
		mStateMachine = new StateMachine_DataChunk(this,mPhysicsMgr,SMName);
	}
	else if(SMName == "SpawnVirus")
	{
		mStateMachine	 = new StateMachine_SpawnVirus(this,mPhysicsMgr,SMName);
	}
	else if(SMName == "CriticalSection")
	{
		mStateMachine	 = new StateMachine_CriticalSection(this,mPhysicsMgr,SMName);
	}//ELSE - INCORRECT TYPE
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("AIAgent::_createStateMachine","Error: Bad Agent AI state machine name! Agent will not make anything!",LOGEXCEPTION);
	}
}

//Memory cleanup
void AIAgent::_release()
{
	Destroy();

	//Cleanup memory
	if(mStateMachine)
	{
		delete	mStateMachine;
		mStateMachine = NULL;
	}
}