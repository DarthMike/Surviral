/*
	Filename: ProjectileAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Thrown agent (bullet) it makes specific action
	Comments: To generalize this agent, a behaviour or state machine could be implemented...
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

/*
	TODO:
		AS LONG AS I ADD NEW AGENTS TO THIS FRAMEWORK, I BEGIN TO NOTICE THE LACK OF SEPARATION BETWEEN 
		DRAWING (GFX) AND PURE LOGIC FOR AGENTS AND GAME. I HAVE TO THINK ABOUT IT AND RESTRUCTURE SOMETHING.
		KIND OF MADE FOR UI, BUT NOT FOR AGENTS...
*/
#ifndef _PROJECTILEAGENT
#define _PROJECTILEAGENT

//Library dependencies

//Classes dependencies
#include "IAgent.h"
#include "Shared_Resources.h"
#include "GFXDefs.h"

//class b2Body;

//Definitions
//Properties to contain from agent - Inherited
struct ProjectileAgentPar : public GameAgentPar
{
		ProjectileAgentPar():
		GameAgentPar(),
		firedby(NULL),
		physicbody(NULL),
		lifetime(2000.f)
		{
			//Determine type
			type = PROJECTILE;
		}
		
		virtual AgentType GetType() const { return type; }
		
		ContainedSprite sprite;
		IAgent* firedby;
		b2Body* physicbody;
		float lifetime;

		//Assignment operator for fast copying
		ProjectileAgentPar& operator=(const ProjectileAgentPar &newparams)
		{
			sprite = newparams.sprite;
			firedby = newparams.firedby;
			physicbody = newparams.physicbody;
			position = newparams.position;
			rotation = newparams.rotation;
			lifetime = newparams.lifetime;
			//Deep copy of elements in struct
			return (*this);
		}
};

class ProjectileAgent : public IAgent
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	ProjectileAgent(const std::string& id,AgentsManager* agentsmgrptr, PhysicsManagerPointer physicsptr):
	  IAgent(id,agentsmgrptr),
	  mActive(false),
	  mPhysicsMgr(physicsptr),
	  mCounter(0.0f),
	  mOutOfLimits(false)
	{
		_init();
	};
	virtual ~ProjectileAgent(){ _release(); }
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
	ProjectileAgentPar mParams;					//All parameters needed to create the agent
	bool mActive;							//Internal "active" tracking
	bool mOutOfLimits;						//Out of limits tracking
	
	PhysicsManagerPointer mPhysicsMgr;			//Physics manager pointer
	float mCounter;							//Lifetime counter

	//---- INTERNAL FUNCTIONS ----
	void _init();
	void _release();								//Release internal resorces
};

#endif