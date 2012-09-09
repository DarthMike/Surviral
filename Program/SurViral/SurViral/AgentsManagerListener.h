/*
	Filename: AgentsManagerListener.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Listener for an agents manager
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _AGENTSMANAGERLISTENER
#define _AGENTSMANAGERLISTENER
//Library dependencies

//Class dependencies
#include "GameEventManager.h"

//Forward declarations
class AgentsManager;
class EventData;

class AgentsManagerListener : public IEventListener
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	AgentsManagerListener(AgentsManager* managerptr)
		:mName("AgentsManagerListener"),
		mAgentsManager(managerptr)
	{
		assert(mAgentsManager);
		//Register events to process
		//Collisions
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NewCollision);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_PersistantCollision);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DeletedCollision);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_CollisionResult);
		//Out of limits
		SingletonGameEventMgr::Instance()->AddListener(this,Event_OutOfLimits);
		//Joint destroyed
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DestroyedJoint);
		//Render additionals events (triggered!)
		SingletonGameEventMgr::Instance()->AddListener(this,Event_RenderInLayer);
		//Create a new agent during game 
		SingletonGameEventMgr::Instance()->AddListener(this,Event_CreateNewAgent);
		//Agent Created notification
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NewAgentCreated);
		//Commands
		SingletonGameEventMgr::Instance()->AddListener(this,Event_MoveCommand); //Move command for player
		SingletonGameEventMgr::Instance()->AddListener(this,Event_ShootCommand); //Shoot command for player
		//GamePlay
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DataBlockInfected);	//DataBlockInfected
		SingletonGameEventMgr::Instance()->AddListener(this,Event_AntiVirusNewTargetRequest);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_VirusTerminated);
	}
	~AgentsManagerListener()
	{
		//Deregister events to process
		//Collisions
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NewCollision);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_PersistantCollision);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DeletedCollision);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_CollisionResult);
		//Out of limits
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_OutOfLimits);
		//Joint destroyed
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DestroyedJoint);
		//Render additionals events (triggered!)
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_RenderInLayer);
		//Create a new agent during game 
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_CreateNewAgent);
		//Agent Created notification
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NewAgentCreated);
		//Commands
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_MoveCommand); //Move command for player
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_ShootCommand); //Shoot command for player
		//GamePlay
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DataBlockInfected);	//DataBlockInfected
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_AntiVirusNewTargetRequest);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_VirusTerminated);
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Implementation of handling
	virtual bool HandleEvent(const EventData& theevent);
	//Implementation of name
	virtual const std::string& GetName()
	{
		return mName;
	}
	//----- PUBLIC VARIABLES ------	

protected:
	//----- INTERNAL VARIABLES -----
	std::string mName;
	AgentsManager* mAgentsManager;
	//----- INTERNAL FUNCTIONS -----
};

#endif
