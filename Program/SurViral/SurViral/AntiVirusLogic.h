/*
	Filename: AntiVirusLogic.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which encapsulates logic related to antivirus part of game (for my fourth game)
	Comments: This is a hack class which makes logic related to this game only. It could be scripted
			  out of the engine, but I dont make big games to make that! Maybe in future... 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _ANTIVIRUSLOGIC
#define _ANTIVIRUSLOGIC

//Library dependencies

//Class dependencies
#include "General_Resources.h"
#include "Shared_Resources.h"
#include "GFXDefs.h"

//Forward declarations
class SVGame;

class AntiVirusLogic
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	AntiVirusLogic(SVGame* gameptr):
	  mScanTime(1000.0f),
	  mCounter(1000.0f),
	  mMinSeparation(0.0f),
	  mMaxSeparation(1.0f),
	  mTarget(false),
	  mTargetRequest(false),
	  mGameptr(gameptr),
	  mTrackK(0.1f),
	  mTrackingPlayer(false)
	{
	}
	~AntiVirusLogic()
	{
		_release();
	}

	//----- GET/SET FUNCTIONS -----
	float GetScanTime() { return mCounter; }
	bool TrackingPlayer() { return mTrackingPlayer; }
	//----- OTHER FUNCTIONS -----
	void Update(float dt);  //Update method
	void BuildFromXML(ticpp::Element* xmlelement);  //Builder function
	//Event handling
	bool HandleEvents(const EventData& theevent);
private:
	//----- INTERNAL VARIABLES -----
	//Pointer to game object
	SVGame* mGameptr;
	//Four sprites make up scanner
	ContainedSprite mScanLineGFX1;
	ContainedSprite mScanLineGFX2;
	ContainedSprite mScanLineGFX3;
	ContainedSprite mScanLineGFX4;

	//One text sprite represents state of scanner
	ContainedSprite mScanText;
	std::string mText;

	//Position of scanner
	bool mTarget;
	bool mTargetRequest;
	bool mTrackingPlayer;
	std::string mTargetId;
	Vector2 mScanPosition;
	Vector2 mTargetPosition;
	float mScanTime;
	float mTrackK;
	float mMinSeparation;
	float mMaxSeparation;
	float mCounter;

	//----- INTERNAL FUNCTIONS -----
	void _release();
	void _resetVariables();
};

#endif _ANTIVIRUSLOGIC