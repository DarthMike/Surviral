/*
	Filename: PlayerAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: An agent for the game character the player controls
	Comments: The player is the mouse "grabber" in this game. 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "PlayerAgent.h"
#include "AgentsManager.h"
#include "IndieLibManager.h"
#include "PhysicsManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"
#include "Camera2D.h"
#include "Math.h"
#include "ResourceManager.h"
#include "ProjectileAgent.h"
#include <sstream>

//Update object status
void PlayerAgent::UpdateState(float)
{
	//IF - Agent is active
	if(!mActive)
		return;

	//Update position of physics body with movement from UI
	if(mPhysicsMgr->IsPhysicsStepped())
	{
		b2Vec2 newposition (mParams.physicbody->GetPosition());
		mParams.position = Vector2(newposition.x,newposition.y);

		b2Vec2 movspeed (static_cast<float>(mMoveDirection.x),
			             static_cast<float>(mMoveDirection.y));

		//IF - It is collided, move slower (not to push indefinitively)
		if(mWorldCollisions > 0)
			movspeed *= mParams.collidedspeed;
		else
			movspeed *= mParams.movespeed;

		mParams.physicbody->SetLinearVelocity(movspeed);
		mParams.physicbody->WakeUp();
	}

	//IF - Exists sprite
	if(mParams.sprite.gfxentity)
	{	//Update gfx
		_updatePosition();
	}//IF

	//Send event of position of player agent
	AgentPositionInfo eventdata(mId,mParams.GetType(),mParams.position);
	SingletonGameEventMgr::Instance()->QueueEvent(
				EventDataPointer(new AgentPositionEvent(Event_AgentPosition,eventdata))
				);

	//Commands
	//Shoot
	if(mShootCommand)
	{
		_processShootCommand();
		mShootCommand = false;
	}

}

//Process possible collisions
bool PlayerAgent::HandleCollision(const CollisionEventData& data)
{	
	bool processed(false);
	
	const ContactInfo& contactinfo(static_cast<const ContactInfo&>(data.GetCollisionData()));

	if(data.GetEventType() == Event_NewCollision)
	{
		if(!contactinfo.collidedshape1->IsSensor()
		   &&
		   !contactinfo.collidedshape2->IsSensor())
		{
				++mWorldCollisions;
		}
	}
	else if(data.GetEventType() == Event_DeletedCollision)
	{
		if(!contactinfo.collidedshape1->IsSensor()
		   &&
		   !contactinfo.collidedshape2->IsSensor())
		{
				--mWorldCollisions;
		}
	}

	return processed;
}

//Process possible events
bool PlayerAgent::HandleEvent(const EventData& data)
{
	bool eventprocessed (false);
	//IF - Move command
	if(data.GetEventType() == Event_MoveCommand)
	{
		const MoveCommandEvent& eventdata(static_cast<const MoveCommandEvent&>(data));
		mMoveDirection = eventdata.GetDirection();
	}//ELSE - Shoot Command
	else if(data.GetEventType() == Event_ShootCommand)
	{
		const ShootCommandEvent& eventdata(static_cast<const ShootCommandEvent&>(data));
		mShootDirection = eventdata.GetDirection();
		mShootCommand = true;
	}//ELSE - Data block infected!
	else if(data.GetEventType() == Event_DataBlockInfected)
	{
		const DataBlockInfectedEvent& eventdata (static_cast<const DataBlockInfectedEvent&>(data));
		//IF - Not Critical
		if(!eventdata.IsCritical())
		{
			Vector2 blockpos (eventdata.GetPosition());

			mParams.spawnaibuilderptr->CreateAIAgent(mPhysicsMgr,true);
			AIAgentPar createdagentpar (mParams.spawnaibuilderptr->GetAIParams());
			//HACK//
			createdagentpar.physicbody->SetXForm(b2Vec2(static_cast<float>(blockpos.x),static_cast<float>(blockpos.y)),createdagentpar.physicbody->GetAngle());
			Vector2 agentpospix (SingletonIndieLib::Instance()->FromCoordToPix(blockpos));
			createdagentpar.sprite.gfxentity->SetPosition(static_cast<float>(agentpospix.x),static_cast<float>(agentpospix.y),createdagentpar.sprite.gfxentity->GetPosZ());
			//HACK END//
		}//IF
	}//ELSE - New Agent Created
	else if(data.GetEventType() == Event_NewAgentCreated)
	{
		const NewAgentCreatedEvent& eventdata(static_cast<const NewAgentCreatedEvent&>(data));

		if(eventdata.GetAgentType() == AI && eventdata.GetAgentAI() == "SpawnVirus")
		{
			std::string agentid (eventdata.GetAgentId());
			mSpawnedViruses.push(mAgentsMgrptr->GetAgent(agentid));
		}
	}//ELSE - Request for new target from antivirus...
	else if(data.GetEventType() ==	Event_AntiVirusNewTargetRequest)
	{
		//Prepare data to target antivirus
		std::string agentid;
		Vector2 position;
		bool isplayer(false);
		//IF - There aren't more spawned viruses....
		if(mSpawnedViruses.empty())
		{
			//ME
			agentid = mId;
			position = mParams.position;
			isplayer = true;
		}//ELSE - Others!
		else
		{
			//Divert antivirus target to other spawned virus
			IAgent* nexttarget((mSpawnedViruses.front()));
			agentid = nexttarget->GetId();
			position = nexttarget->GetPosition();
			mSpawnedViruses.pop();
		}
		
		NewAntiVirusTargetInfo eventinfo(agentid,position,isplayer);
		SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new NewAntiVirusTargetEvent(Event_NewAntiVirusTarget,eventinfo))
											);
	}

			
	return eventprocessed;
}

//Handle out of limits
void PlayerAgent::HandleOutOfLimits(const OutOfLimitsEventData&) 
{ 
	//Nothing
}					

//Create from params
void PlayerAgent::Create( const GameAgentPar *params)	
{
	//Agent params should be of type
	//Copy parameters from creation 
	assert(params->GetType() == PLAYER);
	
	//Cast down params hierarchy to this type of params
	const PlayerAgentPar *pagentparams = static_cast<const PlayerAgentPar*>(params);
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
void PlayerAgent::Destroy()
{
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
void PlayerAgent::_init()
{
}


//Release internal resources
void PlayerAgent::_release()
{
	Destroy();
}

//Display position of sprite
void PlayerAgent::_updatePosition()
{
	//Read position and rotation of body
	b2Vec2 bodypos (mParams.physicbody->GetPosition());
	mParams.position = Vector2 (bodypos.x, bodypos.y);
	mParams.rotation = mParams.physicbody->GetAngle();

	//Update GFX display
	mParams.sprite.gfxentity->SetAngleXYZ(0.0f,0.0f,SingletonMath::RadiansToAngle(mParams.rotation));
	Vector2 positionpix = SingletonIndieLib::Instance()->FromCoordToPix(Vector2(mParams.position.x,mParams.position.y));
	mParams.sprite.gfxentity->SetPosition(static_cast<float>(positionpix.x),static_cast<float>(positionpix.y),0);	
}

void PlayerAgent::_processShootCommand()
{
	//Create new physics body for this projectile
	b2BodyDef bodydef;
	bodydef.allowSleep = true;
	bodydef.fixedRotation = false;
	bodydef.isBullet = true;
	bodydef.position = b2Vec2(static_cast<float>(mParams.position.x),static_cast<float>(mParams.position.y));
	b2Body* newbody (mPhysicsMgr->CreateBody(&bodydef,""));
	//Create a shape for this new projectile
	b2PolygonDef polshapedef;
	polshapedef.filter = mParams.projectilefilterdef;
	polshapedef.density = mParams.projectiledensity;
	polshapedef.restitution = 1.0f; 
	polshapedef.SetAsBox(mParams.projh/2,mParams.projw/2);
	mPhysicsMgr->CreatePolygonShape(&polshapedef,newbody);
	//It is a dynamic object = mass
	newbody->SetMassFromShapes();
	//Apply a force
	mShootDirection*= mParams.shootforce;
	newbody->ApplyImpulse(b2Vec2(static_cast<float>(mShootDirection.x),static_cast<float>(mShootDirection.y)),newbody->GetPosition());
	
	//Create new sprite entity
	SurfacePointer gfxsurface (SingletonResourceMgr::Instance()->GetSurfaceResource(mParams.projectilegfxresource));
	SpritePointer newentity(new IND_Entity2d());
	int targetlayer = SingletonIndieLib::Instance()->GetCamera("General")->GetLayer();  //Render in "General" layer
	SingletonIndieLib::Instance()->Entity2dManager->Add(targetlayer,newentity.get());
	newentity->SetSurface(gfxsurface.get());
	newentity->SetHotSpot(0.5f,0.5f);
	newentity->ShowGridAreas(false); 
	newentity->ShowCollisionAreas(false);
	float scalefactor (SingletonIndieLib::Instance()->GetGeneralScale());
	//Scale to fit requested dimensions
	SingletonIndieLib::Instance()->ScaleToFit(newentity.get(),
											  gfxsurface.get(),
											  static_cast<int>(mParams.projw * scalefactor),
											  static_cast<int>(mParams.projh * scalefactor));
	//Position
	Vector2 pospix = SingletonIndieLib::Instance()->FromCoordToPix(mParams.position);
	newentity->SetPosition(static_cast<float>(pospix.x),static_cast<float>(pospix.y),1000);  //With Z big enough to be over everything in layer
	
	//Once all elements of new projectile agent are created, add them to params and request new agent creation
	ProjectileAgentPar* newagentpar = new ProjectileAgentPar();
	newagentpar->firedby = this;
	newagentpar->physicbody = newbody;
	newagentpar->sprite = ContainedSprite(newentity,Vector2(0,0),0.0f);
	newagentpar->position = mParams.position;
	newagentpar->lifetime = mParams.projectilelifetime;

	AgentParPointer agentparamsptr (newagentpar);
	CreateNewAgentInfo agentinfo(agentparamsptr);
	SingletonGameEventMgr::Instance()->QueueEvent(
					EventDataPointer(new CreateNewAgentEvent(Event_CreateNewAgent,agentinfo))
					);
}