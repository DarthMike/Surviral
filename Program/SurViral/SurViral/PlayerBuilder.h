/*
	Filename: PlayerBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric player agent for the game
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _PLAYERBUILDER
#define _PLAYERBUILDER

//Library dependencies	
#include "boost\shared_ptr.hpp"  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...

#include <string>
//Class dependencies
#include "XMLParser.h"
#include "LogManager.h"
#include "GenericException.h"
#include "Shared_Resources.h"
#include "PlayerAgent.h"
#include "PhysicsBodyBuilder.h"
#include "SpriteBuilder.h"

class PlayerBuilder
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	PlayerBuilder():
	mParamsLoaded(false)
	{
	}
	~PlayerBuilder()
	{
	}
	//----- GET/SET FUNCTIONS -----
	PlayerAgentPar GetPlayerParams() { assert (mParamsLoaded); return mParams; }  //Returns A COPY of the created player params
	//----- OTHER FUNCTIONS -----
	void ReadPlayerParams(const ticpp::Element* xmlelement);   //Read a part of xml info
	void CreatePlayer(AgentsManagerPointer agentsmgrptr, PhysicsManagerPointer physicsmgrptr);   //Create a new sprite
protected:
	//----- INTERNAL VARIABLES -----
	PlayerAgentPar mParams;
	bool mParamsLoaded;

	PhysicsBodyBuilder mBodyBuilder;
	SpriteBuilder mSpriteBuilder;
	//----- INTERNAL FUNCTIONS -----
};

#endif