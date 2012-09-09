/*
	Filename: GameOverlayListener.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Event listener for overlay
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _GAMEOVERLAYLISTENER
#define _GAMEOVERLAYLISTENER
//Library dependencies

//Class dependencies
#include "GameEventManager.h"

//Forward declarations
class GameOverlay;
class EventData;

class GameOverlayListener : public IEventListener
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameOverlayListener(GameOverlay* overlayptr)
		:mName("GameOverlayListener"),
		mOverlay(overlayptr)
	{
		//Other
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DebugString);  //Debug strings
		//GamePlay
		SingletonGameEventMgr::Instance()->AddListener(this,Event_SolidCollision);    //Solid bodies collision
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NewAgentCreated); //Agent created
		SingletonGameEventMgr::Instance()->AddListener(this,Event_AgentPosition); //Agent position changed
		SingletonGameEventMgr::Instance()->AddListener(this,Event_AgentDestroyed); //Agent destroyed
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DataBlockCorrupted);  //Data Block Corrupted
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NewAntiVirusTarget);  //New antivirus target
		//Level events
		SingletonGameEventMgr::Instance()->AddListener(this,Event_LevelCompleted); //Level Completed
		SingletonGameEventMgr::Instance()->AddListener(this,Event_ExitGame);  //Exit game event
		SingletonGameEventMgr::Instance()->AddListener(this,Event_GameStatistics);  //Change in game statistics
		SingletonGameEventMgr::Instance()->AddListener(this,Event_GameOver);  //Game Over
	}
	~GameOverlayListener()
	{
		//Other
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DebugString);  //Debug strings
		//GamePlay
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_SolidCollision);    //Solid bodies collision
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NewAgentCreated); //Agent created
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_AgentPosition); //Agent position changed
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_AgentDestroyed); //Agent destroyed
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DataBlockCorrupted);  //Data Block Corrupted
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NewAntiVirusTarget);  //New antivirus target
		//Level events
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_LevelCompleted); //Level Completed
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_ExitGame);  //Exit game event
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_GameStatistics);  //Change in game statistics
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_GameOver);  //Game Over
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
	GameOverlay* mOverlay;
	//----- INTERNAL FUNCTIONS -----
};

#endif
