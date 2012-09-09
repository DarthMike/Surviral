/*
	Filename: GameEvents.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Definition of specific game-related events
	Comments: Shared pointer (boost smart pointer implementation) used to define events data
	Attribution: "Game coding complete" was the inspiration , great book! http://www.mcshaffry.com/GameCode/
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _GAMEEVENTS
#define _GAMEEVENTS

//Library dependencies
#include <vector>

//Classes dependencies
#include "GameEventsDef.h"
#include "GameLogicDefs.h"
#include "Vector2.h"

typedef struct DebugStringInfo //New debug string info
{
	DebugStringInfo(const std::string& thestring):
	message(thestring)
	{}

	//New target position
	std::string message;
}DebugStringInfo;

typedef struct NewMousePosInfo  //New mouse position info
{
	NewMousePosInfo(const Vector2& positionpix):
	pospix(positionpix)
	{}

	Vector2 pospix;
}NewMousePosInfo;

typedef struct CreateNewAgentInfo   //New agent info
{
	CreateNewAgentInfo(AgentParPointer agentparams, const std::string& agentid = ""):
	params(agentparams),
	agentid(agentid)
	{}

	AgentParPointer params;
	std::string agentid;
}CreateNewAgentInfo;

typedef struct AgentPositionInfo   //New agent created info
{
	AgentPositionInfo(const std::string& agentid, AgentType type, const Vector2& position, const std::string& agentai = ""):
	agentid(agentid),
	type(type),
	position(position),
	agentai(agentai)
	{}

	std::string agentid;
	AgentType type;
	Vector2 position;
	std::string agentai;
}AgentOutInfo;

typedef AgentPositionInfo NewAgentCreatedInfo;  //New agent created info (same as agent position)
typedef AgentPositionInfo AgentOutInfo ;  //Agent out info (the same as agent position)
typedef AgentPositionInfo AgentDestroyedInfo  ;  //Agent destroyed info (the same as agent position)

typedef struct SolidCollisionInfo  //New collision info
{
	SolidCollisionInfo(MaterialType material)
	:
	material(material)
	{}

	MaterialType material;
}SolidCollisionInfo;

typedef struct MoveCommandInfo  //Move command
{
	MoveCommandInfo(const Vector2& direction):
	direction(direction)
	{}

	Vector2 direction;
}MoveCommandInfo;

typedef struct ShootCommandInfo  //Shoot command
{
	ShootCommandInfo(const Vector2& direction ):
	direction(direction)
	{}

	Vector2 direction;
}ShootCommandInfo;

typedef struct DataBlockInfectedInfo  //Data block infected
{
	DataBlockInfectedInfo(const Vector2& pos ,bool iscritical):
	position(pos),
	iscritical(iscritical)
	{}

	Vector2 position;
	bool iscritical;
}DataBlockInfectedInfo;

typedef DataBlockInfectedInfo DataBlockCorruptedInfo;

typedef struct NewAntiVirusTargetInfo  //New antivirus target
{
	NewAntiVirusTargetInfo(const std::string& agentid, const Vector2 position, bool isplayer):
	agentid(agentid),
	position(position),
	isplayer(isplayer)
	{}

	std::string agentid;
	Vector2 position;
	bool isplayer;
}NewAntiVirusTargetInfo;

typedef struct GameStatisticsInfo    //Game statistics 
{
	GameStatisticsInfo(unsigned int seconds, unsigned int minutes, unsigned int scanmils, unsigned int scansecs):
	seconds(seconds),
	minutes(minutes),
	scanmilliseconds(scanmils),
	scanseconds(scansecs)
	{}

	unsigned int seconds, minutes, scanseconds, scanmilliseconds;

}GameStatisticsInfo;

typedef struct RenderInLayerInfo //Perform additional per-layer rendering
{
	RenderInLayerInfo(int thelayer):
	layer(thelayer)
	{}
	int layer;  //Layer to render in
}RenderInLayerInfo;

//DATA WHICH WILL BE SENT  - DEBUG INFO
class DebugMessageEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	DebugMessageEvent(const GameEventType type, const DebugStringInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mDebugInfo(info)
	{}
	virtual ~DebugMessageEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const std::string& GetString() const { return mDebugInfo.message; }
	
protected:
	//----- INTERNAL VARIABLES -----
	DebugStringInfo mDebugInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT -NEW POSITION OF MOUSE
class NewMousePosEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	NewMousePosEvent(const GameEventType type, const NewMousePosInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~NewMousePosEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	Vector2 GetPosPix() const  {return mInfo.pospix;}
	
protected:
	//----- INTERNAL VARIABLES -----
	NewMousePosInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - CREATE NEW AGENT
class CreateNewAgentEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	CreateNewAgentEvent(const GameEventType type, const CreateNewAgentInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~CreateNewAgentEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	AgentParPointer GetAgentInfo() const  {return mInfo.params;}
protected:
	//----- INTERNAL VARIABLES -----
	CreateNewAgentInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - AGENT OUT OF LIMITS
class AgentPositionEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	AgentPositionEvent(const GameEventType type, const AgentPositionInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~AgentPositionEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	std::string GetAgentId() const  {return mInfo.agentid;}
	AgentType GetAgentType() const { return mInfo.type; }	
	Vector2 GetPosition() const { return mInfo.position; }
	std::string GetAgentAI() const { return mInfo.agentai; }
protected:
	//----- INTERNAL VARIABLES -----
	AgentOutInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

typedef AgentPositionEvent AgentOutEvent;
typedef AgentPositionEvent NewAgentCreatedEvent; 
typedef AgentPositionEvent AgentDestroyedEvent;

//DATA WHICH WILL BE SENT - SOLID COLLISION EVENT
class SolidCollisionEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SolidCollisionEvent(const GameEventType type, const SolidCollisionInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~SolidCollisionEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const SolidCollisionInfo& GetInfo() const  {return mInfo;}
	
protected:
	//----- INTERNAL VARIABLES -----
	SolidCollisionInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - MOVE COMMAND
class MoveCommandEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	MoveCommandEvent(const GameEventType type, const MoveCommandInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~MoveCommandEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	Vector2 GetDirection() const  {return mInfo.direction;}
	
protected:
	//----- INTERNAL VARIABLES -----
	MoveCommandInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - SHOOT COMMAND
class ShootCommandEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	ShootCommandEvent(const GameEventType type, const ShootCommandInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~ShootCommandEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	Vector2 GetDirection() const  {return mInfo.direction;}
	
protected:
	//----- INTERNAL VARIABLES -----
	ShootCommandInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - DATABLOCK INFECTED
class DataBlockInfectedEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	DataBlockInfectedEvent(const GameEventType type, const DataBlockInfectedInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~DataBlockInfectedEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	Vector2 GetPosition() const { return mInfo.position; }
	bool IsCritical() const { return mInfo.iscritical; }

protected:
	//----- INTERNAL VARIABLES -----
	DataBlockInfectedInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

typedef DataBlockInfectedEvent DataBlockCorruptedEvent;

//DATA WHICH WILL BE SENT - DATABLOCK INFECTED
class NewAntiVirusTargetEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	NewAntiVirusTargetEvent(const GameEventType type, const NewAntiVirusTargetInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~NewAntiVirusTargetEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	std::string GetAgentId() const { return mInfo.agentid; }
	Vector2 GetPosition() const { return mInfo.position; }
	bool IsPlayer() const {return mInfo.isplayer; }

protected:
	//----- INTERNAL VARIABLES -----
	NewAntiVirusTargetInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - GAME STATISTICS
class GameStatisticsEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameStatisticsEvent(const GameEventType type, const GameStatisticsInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~GameStatisticsEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const GameStatisticsInfo& GetInfo() const { return mInfo; }

protected:
	//----- INTERNAL VARIABLES -----
	GameStatisticsInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - RENDER DATA IN A CONCRETE LAYER
class RenderInLayerXEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	RenderInLayerXEvent(const GameEventType type, const RenderInLayerInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mLayerInfo(info)
	{}
	virtual ~RenderInLayerXEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	int GetLayer() const { return mLayerInfo.layer; }

protected:
	//----- INTERNAL VARIABLES -----
	RenderInLayerInfo mLayerInfo;
	//----- INTERNAL FUNCTIONS -----
};

#endif