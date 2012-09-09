/*
	Filename: SVGame.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Main game class
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
#ifndef _SVGAME
#define _SVGAME

//Library dependencies

//Class dependencies
#include "General_Resources.h"
#include "Shared_Resources.h"
///HACK
#include "AntiVirusLogic.h"
///HACK END

//Forward declarations
class SVGameListener;


class SVGame
{
	friend class SVGameListener; //Listener is friend to this, as it has access to internals... (just a handle really)
	///HACK
	friend class LevelBuilder;
	///HACK END
//Definitions
typedef struct LevelInfo   //Level info structure
{
	//CONSTRUCTOR
	LevelInfo(const std::string& name, bool completed, const std::string& fullpath):
		name(name),
		fullpath(fullpath),
		completed(completed)
	{}

	//Variables
	std::string name;
	std::string fullpath;
	bool completed;
}LevelInfo;

//Containers for level info
typedef std::vector<LevelInfo> LevelsVector;
typedef LevelsVector::iterator LevelsVectorIterator;

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SVGame():
	  mEventListener(NULL),
	  mFirstStart(false),
	  mGameOver(false),
	  mRestartLevel(false),
	  mFirstTickDelayCounter(0.0f),
	  mGameStarted(false),
	  mGameElapsedTime(0.0f),
	  mUpdateCounter(0.0f),
	  mLevelCompleted(false)
	{
		//Init resources, load managers and create game agents
		_init();
	}
	~SVGame()
	{
		_release();
	}

	//----- GET/SET FUNCTIONS -----
	PhysicsManagerPointer GetPhysicsManager() { return mPhysicsMgr; }
	AgentsManagerPointer GetAgentsManager() { return mAgentsManager; }
	//----- OTHER FUNCTIONS -----
	void UpdateGameState(float dt);		//Logic tick update
	void RenderScene();					//Drawing of whole scene
	void LoadFirstLevel();				//Load First Level
	void LoadNextLevel();               //Load Next Level
	void LoadLevel(int index);
	void RestartCurrentLevel();

private:
	//----- INTERNAL VARIABLES -----
	std::string mLevelsFilePath;			//Levels filepath
	GameLevelPointer mCurrentLevelPointer; //Current loaded level data pointer
	static const float FIRSTTICKDELAY;
	float mFirstTickDelayCounter;					//Delay to update first logic tick
	bool mFirstStart;						//Logic tracking for first start of level
	bool mGameOver;							//Game Over tracking
	bool mLoadNextLevel;					//Command to load next level
	bool mRestartLevel;						//Command to restart level
	bool mGameStarted;						//Game started tracking
	bool mLevelCompleted;					//Level completed tracking
	LevelsVector mLevels;					//Levels info container
	PhysicsManagerPointer mPhysicsMgr;	//Physics manager pointer
	AgentsManagerPointer mAgentsManager;	//Agents Manager pointer
	
	SVGameListener* mEventListener;  //Listener for events
	
	//Game Trackings
	float mGameElapsedTime;   //Elapsed game time
	float mUpdateCounter;	  //Counter to update from time to time
	AntiVirusLogic* mAVLogic;  //Encapsulated logic to game, related to antivirus...

	//----- INTERNAL FUNCTIONS -----
	//Init and release
	void _init();
	void _release();
	void _loadLevels();
	//Load a level, reset managers and restart internal logic variables
	void _resetAndPrepareLevel(const std::string& levelname, const std::string& levelfilepath);

	//Event handling
	bool _handleEvents(const EventData& theevent);
};

#endif