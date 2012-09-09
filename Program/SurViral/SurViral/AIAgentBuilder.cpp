/*
	Filename: AIAgentBuilder.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric AI agent
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "AIAgentBuilder.h"
#include "AgentsManager.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"

//Definition of static elements
int AIAgentBuilder::mAgentsBuilt = 0;

//Read a part of xml info
void AIAgentBuilder::ReadAIParams(const ticpp::Element* xmlelement)
{
	/*AIAGENT ELEMENT
 	Element: AIAGENT Atts: AI(string) SteerForce(number) MaxSpeedX(number) MaxSpeedY(number)
			 Element: Sprite
			 Element: Body
	*/
	
	//-------Get attributes from entity---------
	//AI to use
	std::string ai (xmlelement->GetAttribute("AI"));	
	//Check correctness of parameters
	if(ai == "" )
		throw GenericException("Player Element, bad attributes detected! - Out of bounds values",GenericException::FILE_CONFIG_INCORRECT);

	//Store params
	mParams.agentAI = ai;

	//------Get elements associated to entity-------
	ticpp::Iterator <ticpp::Element> entelement;
	//LOOP - Get elements of entity
	for(entelement = entelement.begin(xmlelement);entelement != entelement.end();entelement++)
	{
		//IF - FOUND BODY ELEMENT
		if(entelement->Value() == "Body")
		{
			mBodyBuilder.ReadBodyParams(entelement.Get());
		}//ELSE - FOUND SPRITE ELEMENT
		else if(entelement->Value() == "Sprite")
		{
			mSpriteBuilder.ReadSpriteParams(entelement.Get());
		}
	}//LOOP END

	mParamsLoaded = true;
}
	
//Create new agent - If refactoring, a new name is chosen for everything (to create a new element from 
//a template in this builder with different procedurally-generated name)
void AIAgentBuilder::CreateAIAgent(PhysicsManagerPointer physicsmgrptr, bool refactor)
{
	assert (mParamsLoaded);

	//Create body first
	mBodyBuilder.CreateNewBody(physicsmgrptr,refactor);
	//Store body definition to agent
	mParams.physicbody = mBodyBuilder.GetCreatedBody();

	//Create sprite secondly
	mSpriteBuilder.CreateNewSprite();
	//Store sprite definition to agent
	mParams.sprite = mSpriteBuilder.GetCreatedSprite();

	CreateNewAgentInfo agentinfo(AgentParPointer(new AIAgentPar(mParams)));
	SingletonGameEventMgr::Instance()->QueueEvent(
					EventDataPointer(new CreateNewAgentEvent(Event_CreateNewAgent,agentinfo))
					);
	++mAgentsBuilt;

	SingletonLogMgr::Instance()->AddNewLine("AIAgentBuilder","AI agent created",LOGDEBUG);
}

