/*
	Filename: PlayerBuilder.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric player agent for the game
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "PlayerBuilder.h"
#include "AgentsManager.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"
#include "AIAgentBuilder.h"

//Read a part of xml info
void PlayerBuilder::ReadPlayerParams(const ticpp::Element* xmlelement)
{
	/*PLAYER ELEMENT
 	Element: PLAYER Atts: MoveSpeed(number) CollidedSpeed(number) ShootForce(number)
                    ProjectileCollGroupIndex (number) ProjectileCollMaskBits(number) ProjectileCollCategoryBits(number) ProjectileImageName(number)
                    ProjectileHeight(number) ProjectileWidth(number) ProjectileDensity(number) ProjectileLifeTime(number)
			 Elements: Body
					   Sprite
					   SpawnAIDef
			 
	*/
	
	//-------Get attributes from entity---------
	float movespeed, collidedspeed, shootforce;
	std::string projectileimgname;
	float projh,projw,projdensity, lifetime;
	xmlelement->GetAttribute("MoveSpeed",&movespeed);
	xmlelement->GetAttribute("CollidedSpeed",&collidedspeed);
	xmlelement->GetAttribute("ShootForce",&shootforce);
	projectileimgname = xmlelement->GetAttribute("ProjectileImageName");
	xmlelement->GetAttribute("ProjectileCollGroupIndex",&mParams.projectilefilterdef.groupIndex);
	xmlelement->GetAttribute("ProjectileCollCategoryBits",&mParams.projectilefilterdef.categoryBits);
	xmlelement->GetAttribute("ProjectileCollMaskBits",&mParams.projectilefilterdef.maskBits);
	xmlelement->GetAttribute("ProjectileDensity",&projdensity);
	xmlelement->GetAttribute("ProjectileHeight",&projh);
	xmlelement->GetAttribute("ProjectileWidth",&projw);
	xmlelement->GetAttribute("ProjectileLifeTime",&lifetime);

	//Check correctness of parameters
	if(movespeed <= 0.0f || collidedspeed <= 0.0f || shootforce < 0.0f || projdensity <= 0.0f || projh <= 0.0f || projw <= 0.0f || lifetime <= 0.0f)
		throw GenericException("Player Element, bad attributes detected! - Out of bounds values",GenericException::FILE_CONFIG_INCORRECT);

	if(!SingletonResourceMgr::Instance()->GetSurfaceResource(projectileimgname))
		throw GenericException("Player Element, bad attributes detected! - Projectile image resource '" + projectileimgname+ "' not loaded!",GenericException::FILE_CONFIG_INCORRECT);

	//Store params
	mParams.movespeed = movespeed;
	mParams.collidedspeed = collidedspeed;
	mParams.shootforce = shootforce;
	mParams.projectilegfxresource = projectileimgname;
	mParams.projectiledensity = projdensity;
	mParams.projh = projh;
	mParams.projw = projw;
	mParams.projectilelifetime = lifetime * 1000.0f;  //In ms

	//------Get elements associated to entity-------
	ticpp::Element* childelement;
	//A BODY ELEMENT
	childelement = xmlelement->FirstChildElement("Body");
	mBodyBuilder.ReadBodyParams(childelement);
	//A SPRITE ELEMENT
	childelement = xmlelement->FirstChildElement("Sprite");
	mSpriteBuilder.ReadSpriteParams(childelement);
	//A SPAWNAIDEF ELEMENT
	childelement = xmlelement->FirstChildElement("SpawnAIDef");
	{
		mParams.spawnaibuilderptr = AIAgentBuilderPointer (new AIAgentBuilder());
		mParams.spawnaibuilderptr->ReadAIParams(childelement);
	}

	mParamsLoaded = true;
}
	
//Create new player
void PlayerBuilder::CreatePlayer(AgentsManagerPointer agentsmgrptr, PhysicsManagerPointer physicsmgrptr)
{
	assert (mParamsLoaded);

	//Create body first
	mBodyBuilder.CreateNewBody(physicsmgrptr);
	//Store body definition to agent
	mParams.physicbody = mBodyBuilder.GetCreatedBody();

	//Create sprite secondly
	mSpriteBuilder.CreateNewSprite();
	//Store sprite definition to agent
	mParams.sprite = mSpriteBuilder.GetCreatedSprite();

	//Send event to create agent
	CreateNewAgentInfo agentinfo(AgentParPointer(new PlayerAgentPar(mParams)));
	SingletonGameEventMgr::Instance()->QueueEvent(
					EventDataPointer(new CreateNewAgentEvent(Event_CreateNewAgent,agentinfo))
					);

 	//Create player agent in agents manager
	//agentsmgrptr->CreateNewAgent("PLAYER",&mParams);
	SingletonLogMgr::Instance()->AddNewLine("PlayerBuilder","Player created",LOGDEBUG);
}

