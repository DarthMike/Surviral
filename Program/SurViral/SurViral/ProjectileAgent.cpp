/*
	Filename: ProjectileAgent.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Thrown agent (bullet) it makes action specific
	Comments: To generalize this agent, a behaviour or state machine could be implemented...
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#include "ProjectileAgent.h"
#include "PhysicsManager.h"
#include "IndieLibManager.h" 
#include "GameEventManager.h"
#include "GameEvents.h"
#include "PhysicsEvents.h"
#include "Math.h"
#include "AIAgent.h"

//Update object status
void ProjectileAgent::UpdateState(float dt)
{
	//IF - Agent is active
	if(!mActive)
		return;

	mCounter += dt;

	//Read position and rotation of body
	b2Vec2 bodypos (mParams.physicbody->GetPosition());
	mParams.position = Vector2 (bodypos.x, bodypos.y);
	mParams.rotation = mParams.physicbody->GetAngle();

	//Update GFX display
	mParams.sprite.gfxentity->SetAngleXYZ(0.0f,0.0f,SingletonMath::RadiansToAngle(mParams.rotation));
	Vector2 positionpix = SingletonIndieLib::Instance()->FromCoordToPix(Vector2(mParams.position.x,mParams.position.y));
	mParams.sprite.gfxentity->SetPosition(static_cast<float>(positionpix.x),static_cast<float>(positionpix.y),0);

	if(mOutOfLimits || mCounter >= mParams.lifetime)
	{
		//Just Destroy agent
		Destroy(); 
	}
}

//Process possible collisions
bool ProjectileAgent::HandleCollision(const CollisionEventData& data)
{	
	bool processed(false);
	
	const ContactInfo& collinfo (data.GetCollisionData());
	

	//IF - Result of collision
	if(data.GetEventType() == Event_CollisionResult)
	{
		//IF - Collided with AI agent
		AgentType agent1type(collinfo.agent1->GetType());
		AgentType agent2type(collinfo.agent2->GetType());
		if(agent1type == AI
			||
			agent2type == AI)
		{
			//Send event of solid body collision
			AIAgent* collidedaiagent (NULL);
			if(collinfo.agent1 != this && agent1type == AI)
			{
				collidedaiagent = static_cast<AIAgent*>(collinfo.agent1);
			}
			else if(collinfo.agent2 != this && agent2type == AI)
			{
				collidedaiagent = static_cast<AIAgent*>(collinfo.agent2);
			}

			//IF - Check which specific ai type collided with
			if(collidedaiagent)
			{
				std::string aitype(collidedaiagent->GetAIType());
				//Check which type of agent Ai and send collision event accordingly
				if(aitype == "DataChunk")
				{
					SolidCollisionInfo eventinfo(DATACHUNK);
					SingletonGameEventMgr::Instance()->QueueEvent(
														EventDataPointer(new SolidCollisionEvent(Event_SolidCollision,eventinfo))
														);
				}
				else if (aitype == "CriticalSection")
				{
					SolidCollisionInfo eventinfo(CRITICALSECTION);
					SingletonGameEventMgr::Instance()->QueueEvent(
														EventDataPointer(new SolidCollisionEvent(Event_SolidCollision,eventinfo))
														);
				}
			}//IF
		}
		else if(agent1type == PHYSICBODY
			||
			agent2type == PHYSICBODY)
		{
			//Send generic collision event
			SolidCollisionInfo eventinfo(GENERIC);
					SingletonGameEventMgr::Instance()->QueueEvent(
														EventDataPointer(new SolidCollisionEvent(Event_SolidCollision,eventinfo))
														);
		}

	}

	return processed;
}

//Process possible events
bool ProjectileAgent::HandleEvent(const EventData&)
{
	bool eventprocessed (false);

	return eventprocessed;
}

//Handle out of limits
void ProjectileAgent::HandleOutOfLimits(const OutOfLimitsEventData&) 
{ 
	//Nothing
	mOutOfLimits = true;
}					

//Create from params
void ProjectileAgent::Create( const GameAgentPar *params)	
{
	//Agent params should be of type
	//Copy parameters from creation 
	assert(params->GetType() == PROJECTILE);
	
	//Cast down params hierarchy to this type of params
	const ProjectileAgentPar *pagentparams = static_cast<const ProjectileAgentPar*>(params);
	//Copy all parameters
	mParams = *pagentparams;
	
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

	//Finally, update internal tracking
	mActive = true;
}

//Destroy agent
void ProjectileAgent::Destroy()
{
	if(!mActive)
		return;

	//Deregister graphics entity from indielib
	if(mParams.sprite.gfxentity)
	{
		SingletonIndieLib::Instance()->Entity2dManager->Delete(mParams.sprite.gfxentity.get());
	}

	//Delete bodies data
	std::string bodyname (mPhysicsMgr->GetBodyName(mParams.physicbody));
	mPhysicsMgr->DestroyBody(bodyname);

	//Finally, update internal tracking
	mActive = false;
}

//Init
void ProjectileAgent::_init()
{
}


//Release internal resources
void ProjectileAgent::_release()
{
	Destroy();
}