/*
	Filename: AIAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Agent controlled by AI
	Comments: Internally, it holds a state machine
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*	TODO:
		- ADD SAVE AND LOAD CAPABILITIES; NOW IT IS NOT TOP PRIORITY FOR MY GAMES...
// TODO: ADDED TEMP REFERENCE TO PHYSICS MANAGER BECAUSE OF HACKS IN FORCES!! NEED TO ADD A FORCE CONTROLLER INSIDE
*/

#ifndef _AIAGENT
#define _AIAGENT

//Library dependencies
#include <assert.h>
//Classes dependencies
#include "IAgent.h"
#include "Math.h"
#include "Shared_Resources.h"
#include "GFXDefs.h"
#include "AnimationController.h"
#include "StateMachine.h"

//Forward declarations
class b2Body;
class b2Joint;

//Definitions
//Properties to contain from agent - Inherited
struct AIAgentPar : public GameAgentPar
{
		AIAgentPar():
		GameAgentPar(),
		agentAI(""),
		physicbody(NULL)
		{
			//Determine type
			type = AI;
		}
		virtual AgentType GetType() const { return type; }

		//Agent AI name - for creation of state machine
		std::string agentAI;
		ContainedSprite sprite;  //The graphics entity related
		b2Body* physicbody;				//Physics body related
		
		//Assignment operator for fast copying
		AIAgentPar& operator=(const AIAgentPar &newparams)
		{
			//Deep copy of elements in struct
			agentAI = newparams.agentAI;
			sprite = newparams.sprite;
			physicbody = newparams.physicbody;
			return (*this);
		}
};

//Forward declarations
class PhysicsManager;  //Physics manager

class AIAgent : public IAgent
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	AIAgent(const std::string& id,AgentsManager* agentsmgrptr, PhysicsManagerPointer physicsptr):
    IAgent(id,agentsmgrptr),
    mPhysicsMgr(physicsptr),
	mStateMachine(NULL),
	mActive(false),
	mStateChange(false),
	mOutOfLimits(false)
	{
	}
	virtual ~AIAgent()
	{
		//Release
		_release();
	}
	//----- VALUES GET/SET ---------------
	virtual AgentType GetType() { return mParams.GetType(); }						//Get type of agent
	virtual bool IsAlive()  { return mActive; }             //Get if agent was destroyed
	virtual Vector2 GetPosition() {return mParams.position; }			//Get Position of agent
	virtual float GetRotation() {return mParams.rotation; }		//Get rotation of agent
	//State machine
	StateMachine<AIAgent>* GetStateMachine() { return mStateMachine;} 
	std::string GetAIType() { return mParams.agentAI; }
	//Internal variables access
	AnimationControllerPointer GetAnimationController() { return mAnimController; }
	b2Body* GetPhysicalBody() { return mParams.physicbody; }
	SpritePointer GetGFXSprite() { return mParams.sprite.gfxentity; }
	//----- OTHER FUNCTIONS --------------
	virtual void UpdateState(float dt);								//Update object status
	virtual bool HandleCollision(const CollisionEventData& data);	//Process possible collisions
	virtual bool HandleEvent(const EventData& data);				//Process possible events
	virtual void HandleOutOfLimits(const OutOfLimitsEventData& data);	//Handle out of limits
	virtual void Create( const GameAgentPar *params);				//Create from params
	virtual void Destroy();		//Destroy

	void ChangeState(const std::string& newstatename);   //State machine state change
protected:
	//---- INTERNAL VARIABLES ----
	AIAgentPar mParams;						//All parameters needed to create the agent
	StateMachine<AIAgent>* mStateMachine;	//Internal state machine (depends on type of agent)
	bool mActive;							//Internal "active" tracking
	bool mStateChange;						//Memo of state change request
	bool mOutOfLimits;						//Out of limits memo
	std::string mNextStateRequested;			//Next state requested
	AnimationControllerPointer mAnimController;	//GFX entity animation controller

	PhysicsManagerPointer mPhysicsMgr;
	
	//---- INTERNAL FUNCTIONS ----
	void _createStateMachine( const std::string& agentAI );
	void _release();
};

#endif
