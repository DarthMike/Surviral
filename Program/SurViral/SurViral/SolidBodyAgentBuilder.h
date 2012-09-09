/*
	Filename: SolidBodyAgentBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric SolidBody agent
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _SBAGENTBUILDER
#define _SBAGENTBUILDER

//Library dependencies	
#include <string>
//Class dependencies
#include "XMLParser.h"
#include "LogManager.h"
#include "GenericException.h"
#include "Shared_Resources.h"
#include "SolidBodyAgent.h"
#include "PhysicsBodyBuilder.h"
#include "SpriteBuilder.h"

class SolidBodyAgentBuilder
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SolidBodyAgentBuilder():
	mParamsLoaded(false)
	{
	}
	~SolidBodyAgentBuilder()
	{
	}
	//----- GET/SET FUNCTIONS -----
	SolidBodyPar GetAgentParams() { assert (mParamsLoaded); return mParams; }  //Returns A COPY of the created player params
	
	//----- OTHER FUNCTIONS -----
	void ReadParams(const ticpp::Element* xmlelement);   //Read a part of xml info
	void CreateAgent(PhysicsManagerPointer physicsmgrptr, bool refactor = false);   //Create a new agent
protected:
	//----- INTERNAL VARIABLES -----
	SolidBodyPar mParams;
	std::string mAgentId;
	bool mParamsLoaded;
	static int mAgentsBuilt;

	PhysicsBodyBuilder mBodyBuilder;
	SpriteBuilder mSpriteBuilder;
	//----- INTERNAL FUNCTIONS -----
};

#endif