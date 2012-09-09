/*
	Filename: SVGameListener.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Event listener for game
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _SVGAMELISTENER
#define _SVGAMELISTENER
//Library dependencies

//Class dependencies
#include "GameEventManager.h"

//Forward declarations
class SVGame;
class EventData;

class SVGameListener : public IEventListener
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SVGameListener(SVGame* gameptr)
		:mName("SVGameListener"),
		mGame(gameptr)
	{
		SingletonGameEventMgr::Instance()->AddListener(this,Event_GameStart);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_RestartLevel);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NextLevel);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NewAgentCreated);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_AgentDestroyed);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_AgentPosition); 
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NewAntiVirusTarget); 
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DataBlockCorrupted);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DataBlockInfected);
	}
	~SVGameListener()
	{
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_GameStart);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_RestartLevel);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NextLevel);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NewAgentCreated);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_AgentDestroyed);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_AgentPosition); 
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NewAntiVirusTarget);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DataBlockCorrupted);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DataBlockInfected);
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
	SVGame* mGame;
	//----- INTERNAL FUNCTIONS -----
};

#endif
