/*
	Filename: SolidBodyAgentBuilder.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric SolidBody agent
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "SolidBodyAgentBuilder.h"
#include "AgentsManager.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"

//Definition of static elements
int SolidBodyAgentBuilder::mAgentsBuilt = 0;

//Read a part of xml info
void SolidBodyAgentBuilder::ReadParams(const ticpp::Element* xmlelement)
{
	/*AIAGENT ELEMENT
 	Element: AIAGENT Atts: AI(string) SteerForce(number) MaxSpeedX(number) MaxSpeedY(number)
			 Element: Sprite
			 Element: Body
	*/
	
	//-------Get attributes from entity---------
	//AI to use
	std::string mAgentId (xmlelement->GetAttribute("Id"));
	float wetness(0.5f);
	xmlelement->GetAttribute("Wetness",&wetness,false);  //optional param
	std::string material (xmlelement->GetAttribute("Material"));  
	
	if(mAgentId == "")
		throw GenericException("Bad Id for solidbody agent",GenericException::FILE_CONFIG_INCORRECT);
	if(material != "" && material != "Wood" && material != "Metal" && material != "Stone" )
		throw GenericException("Bad material attribute for'" + mAgentId + "' (Must be empty, or: 'Metal', 'Wood', 'Stone'",GenericException::FILE_CONFIG_INCORRECT);

	if(material != "" && (wetness > 1.0f || wetness < 0.0f)) 
		throw GenericException("Bad wetness attribute for'" + mAgentId + "' Value in range [0,1]",GenericException::FILE_CONFIG_INCORRECT);

	mParams.wetness = wetness;

	//Material
	if(material == "Wood")
		mParams.material = WOODISH;
	else if(material == "Metal")
		mParams.material = METALLIC;
	else if(material == "Stone")
		mParams.material = STONE;
	else
		mParams.material = GENERIC;


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
void SolidBodyAgentBuilder::CreateAgent(PhysicsManagerPointer physicsmgrptr, bool refactor)
{
	assert (mParamsLoaded);

	//Create body first
	mBodyBuilder.CreateNewBody(physicsmgrptr,refactor);
	//Store body definition to agent
	mParams.physicbody = mBodyBuilder.GetCreatedBody();

	//Create sprite secondly
	mSpriteBuilder.CreateNewSprite();
	//Store sprite definition to agent
	mParams.gfxentities.push_back(mSpriteBuilder.GetCreatedSprite());

	std::string sendid = refactor ? "" : mAgentId;
	CreateNewAgentInfo agentinfo(AgentParPointer(new SolidBodyPar(mParams)),sendid);
	SingletonGameEventMgr::Instance()->QueueEvent(
					EventDataPointer(new CreateNewAgentEvent(Event_CreateNewAgent,agentinfo))
					);
	++mAgentsBuilt;

	SingletonLogMgr::Instance()->AddNewLine("SolidBodyAgentBuilder","SolidBody agent created",LOGDEBUG);
}

