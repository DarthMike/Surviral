/*
	Filename: PlayerAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: An agent for the game character the player controls
	Comments: The player is the mouse "grabber" in this game. 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _PLAYERAGENT
#define _PLAYERAGENT

//Library dependencies
#include <queue>
//Classes dependencies
#include "IAgent.h"
#include "Shared_Resources.h"
#include "GFXDefs.h"
#include "AIAgentBuilder.h"

//Definitions
//Properties to contain from agent - Inherited
struct PlayerAgentPar : public GameAgentPar
{
		PlayerAgentPar():
		GameAgentPar(),
		movespeed(0.0f),
		shootforce(1.0f),
		projh(1.0f),
		projw(1.0f),
		projectiledensity(1.0f),
		projectilelifetime(0.5f)
		{
			//Determine type
			type = PLAYER;
		}
		virtual AgentType GetType() const { return type; }
		ContainedSprite sprite;
		float movespeed;
		float collidedspeed;
		b2Body* physicbody;

		//Shooting params
		float shootforce;
		std::string projectilegfxresource;
		float projh,projw;
		float projectiledensity;
		b2FilterData projectilefilterdef;
		float projectilelifetime;

		//Spawn AI builder
		AIAgentBuilderPointer spawnaibuilderptr;

		//Assignment operator for fast copying
		PlayerAgentPar& operator=(const PlayerAgentPar &newparams)
		{
			//Deep copy of elements in struct
			sprite = newparams.sprite;
			movespeed = newparams.movespeed;
			collidedspeed = newparams.collidedspeed;
			shootforce = newparams.shootforce;
			physicbody = newparams.physicbody;
			projectilegfxresource = newparams.projectilegfxresource;
			projh = newparams.projh;
			projw = newparams.projw;
			projectiledensity = newparams.projectiledensity;
			projectilefilterdef.categoryBits = newparams.projectilefilterdef.categoryBits;
			projectilefilterdef.groupIndex = newparams.projectilefilterdef.groupIndex;
			projectilefilterdef.maskBits = newparams.projectilefilterdef.maskBits;
			projectilelifetime = newparams.projectilelifetime;
			spawnaibuilderptr = newparams.spawnaibuilderptr;
			return (*this);
		}
};

class PlayerAgent : public IAgent
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	PlayerAgent(const std::string& id,AgentsManager* agentsmgrptr, PhysicsManagerPointer physicsptr):
	  IAgent(id,agentsmgrptr),
	  mActive(false),
	  mPhysicsMgr(physicsptr),
	  mShootCommand(false),
	  mWorldCollisions(0)
	{
		_init();
	};
	virtual ~PlayerAgent(){ _release(); }
	//----- VALUES GET/SET ---------------
	virtual AgentType GetType() { return mParams.GetType(); }			//Get type of agent
	virtual bool IsAlive()  { return (mActive); }             //Get if agent was destroyed
	virtual Vector2 GetPosition() {return mParams.position; }			//Get Position of agent
	virtual float GetRotation() {return mParams.rotation; }		//Get rotation of agent
	//----- OTHER FUNCTIONS --------------
	//Interface implementations
	virtual void UpdateState(float dt);								//Update object status
	virtual bool HandleCollision(const CollisionEventData& data);	//Process possible collisions
	virtual bool HandleEvent(const EventData& data);				//Process possible events
	virtual void HandleOutOfLimits(const OutOfLimitsEventData& data);								//Handle out of limits
	virtual void Create( const GameAgentPar *params);				//Create from params
	virtual void Destroy();											//Destroy body

protected:
	//---- INTERNAL VARIABLES ----
	PlayerAgentPar mParams;					//All parameters needed to create the agent
	bool mActive;							//Internal "active" tracking
	PhysicsManagerPointer mPhysicsMgr;			//Physics manager pointer

	Vector2 mMoveDirection;					//Move control direction
	Vector2 mShootDirection;				//Shoot direction
	bool mShootCommand;
	int mWorldCollisions;                   //Number of collisions with elements in world

	std::queue<IAgent*> mSpawnedViruses;    //Pointer list to all created viruses

	//---- INTERNAL FUNCTIONS ----
	void _init();
	void _release();								//Release internal resorces
	void _updatePosition();							//Update GFX position
	void _processShootCommand();
};

#endif
