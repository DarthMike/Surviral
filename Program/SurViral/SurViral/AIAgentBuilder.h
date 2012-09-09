/*
	Filename: AIAgentBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric AI agent
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _AIAGENTBUILDER
#define _AIAGENTBUILDER

//Library dependencies	
#include "boost\shared_ptr.hpp"  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...

#include <string>
//Class dependencies
#include "XMLParser.h"
#include "LogManager.h"
#include "GenericException.h"
#include "Shared_Resources.h"
#include "AIAgent.h"
#include "PhysicsBodyBuilder.h"
#include "SpriteBuilder.h"

class AIAgentBuilder
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	AIAgentBuilder():
	mParamsLoaded(false)
	{
	}
	~AIAgentBuilder()
	{
	}
	//----- GET/SET FUNCTIONS -----
	AIAgentPar GetAIParams() { assert (mParamsLoaded); return mParams; }  //Returns A COPY of the created player params
	
	//----- OTHER FUNCTIONS -----
	void ReadAIParams(const ticpp::Element* xmlelement);   //Read a part of xml info
	void CreateAIAgent(PhysicsManagerPointer physicsmgrptr, bool refactor = false);   //Create a new agent
protected:
	//----- INTERNAL VARIABLES -----
	AIAgentPar mParams;
	bool mParamsLoaded;
	static int mAgentsBuilt;

	PhysicsBodyBuilder mBodyBuilder;
	SpriteBuilder mSpriteBuilder;
	//----- INTERNAL FUNCTIONS -----
};

//A shared pointer of this builder
typedef boost::shared_ptr<AIAgentBuilder> AIAgentBuilderPointer;

#endif