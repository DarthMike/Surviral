/*
	Filename: AntiVirusLogic.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which encapsulates logic related to antivirus part of game (for my fourth game)
	Comments: This is a hack class which makes logic related to this game only. It could be scripted
			  out of the framework, but I dont make big games to make that! Maybe in future... 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/	

#include "AntiVirusLogic.h"
#include "SVGame.h"
#include "SpriteBuilder.h"
#include "IndieLibManager.h"
#include "AgentsManager.h"
#include "GameEvents.h"
#include "GameEventManager.h"

//Update method
void AntiVirusLogic::Update(float dt)
{
	
	///**********SMALL STATE MACHINE IMPLEMENTATION FOR THE GAME ANTIVIRUS SCAN LOGIC********
	
	//Update Tracking to target
	if(mTarget)
	{
		mTargetRequest = false;
		Vector2 targetdiff = mTargetPosition - mScanPosition;
		//IF - Significant difference
		if(abs(targetdiff.x) > 0.1 || abs(targetdiff.y) > 0.1)
		{
			Vector2 movespeed = targetdiff * mTrackK;
			mScanPosition += (movespeed * dt);
		}//IF

		//IF - Finished Scanning
		if(mCounter <= 0.0f)
		{
			mCounter = 0.0f;
			
			IAgent* targetagent(mGameptr->GetAgentsManager()->GetAgent(mTargetId));
			//TERMINATE!
			targetagent->Destroy();
			//Send event that scanning is made
			SingletonGameEventMgr::Instance()->QueueEvent(
									EventDataPointer (new EventData(Event_VirusTerminated))
									);
			mTarget = false;
			mText = "SCAN COMPLETE";
		}//ELSE - Not finished scanning
		else
		{
			mCounter -= dt;
		}//IF

		mText = "SCANNING...";
	}//ELSE - Without target
	else if (!mTargetRequest)	
	{
		//Send event to request new follow scan coordinates
		SingletonGameEventMgr::Instance()->QueueEvent(
								EventDataPointer (new EventData(Event_AntiVirusNewTargetRequest))
								);
		mTargetRequest = true;
		mText = "SEARCHING...";
	}//IF



	//Update GFX display...
	assert (mScanTime > 0.0001f);
	float separationratio ((mCounter / mScanTime));

	Vector2 newdistance((separationratio * mMaxSeparation) + mMinSeparation , 0);

	newdistance = SingletonIndieLib::Instance()->FromCoordToPix(newdistance);
	Vector2 scanpospix = SingletonIndieLib::Instance()->FromCoordToPix(mScanPosition);

	int orderz(mScanLineGFX1.gfxentity->GetPosZ());
	mScanLineGFX1.gfxentity->SetPosition(static_cast<float>(scanpospix.x + newdistance.x), static_cast<float>(scanpospix.y),orderz);
	orderz = mScanLineGFX2.gfxentity->GetPosZ();
	mScanLineGFX2.gfxentity->SetPosition(static_cast<float>(scanpospix.x - newdistance.x), static_cast<float>(scanpospix.y),orderz);
	orderz = mScanLineGFX3.gfxentity->GetPosZ();
	mScanLineGFX3.gfxentity->SetPosition(static_cast<float>(scanpospix.x), static_cast<float>(scanpospix.y + newdistance.x),orderz);
	orderz = mScanLineGFX4.gfxentity->GetPosZ();
	mScanLineGFX4.gfxentity->SetPosition(static_cast<float>(scanpospix.x), static_cast<float>(scanpospix.y - newdistance.x),orderz);

	orderz = mScanText.gfxentity->GetPosZ();
	mScanText.gfxentity->SetPosition(static_cast<float>(scanpospix.x - 100), static_cast<float>(scanpospix.y + 100),orderz);
	mScanText.gfxentity->SetText(const_cast<char*>(mText.c_str()));   //INDIELIB PROBLEM WITH CONST-NESS

 
	//***************************************************************************************
}
	
//Builder function
void AntiVirusLogic::BuildFromXML(ticpp::Element* xmlelement)
{
	///CREATION HACKS: Half hardcoded element for antivirus scan. 
	/*ANTIVIRUSSCANNER ELEMENT: Attributes: ScanTime (number) MinSeparation(number) MaxSeparation(number)
		Element(x4): Sprite (with animation or image)
		Element(x1): Sprite (with font)
	*/

	xmlelement->GetAttribute("ScanTime",&mScanTime);
	mCounter = mScanTime;
	xmlelement->GetAttribute("MinSeparation",&mMinSeparation);
	xmlelement->GetAttribute("MaxSeparation",&mMaxSeparation);
	xmlelement->GetAttribute("TrackConstant",&mTrackK);
	
	//Correctness checking
	if(mScanTime < 0.5f || mMinSeparation < 1.0f || mMaxSeparation < mMinSeparation || mTrackK <= 0.0f)
		throw GenericException("Error while loading AntiVirus element in level config file, out of bound values",GenericException::FILE_CONFIG_INCORRECT);

	//Get all 5 sprite elements (ugly...)
	ticpp::Element* nextelement(xmlelement->FirstChildElement("Sprite"));
	SpriteBuilder spbuilder;
	//First line
	spbuilder.ReadSpriteParams(nextelement);
	spbuilder.CreateNewSprite();
	mScanLineGFX1 = spbuilder.GetCreatedSprite();

	//Second line
	nextelement = nextelement->NextSiblingElement("Sprite");
	spbuilder.ReadSpriteParams(nextelement);
	spbuilder.CreateNewSprite();
	mScanLineGFX2 = spbuilder.GetCreatedSprite();

	//Third line
	nextelement = nextelement->NextSiblingElement("Sprite");
	spbuilder.ReadSpriteParams(nextelement);
	spbuilder.CreateNewSprite();
	mScanLineGFX3 = spbuilder.GetCreatedSprite();

	//Fourth line
	nextelement = nextelement->NextSiblingElement("Sprite");
	spbuilder.ReadSpriteParams(nextelement);
	spbuilder.CreateNewSprite();
	mScanLineGFX4 = spbuilder.GetCreatedSprite();
	
	//Text
	nextelement = nextelement->NextSiblingElement("Sprite");
	spbuilder.ReadSpriteParams(nextelement);
	spbuilder.CreateNewSprite();
	mScanText = spbuilder.GetCreatedSprite();

	//Check correctness 
	if(
		(!mScanLineGFX1.gfxentity->GetSurface() && !mScanLineGFX1.gfxentity->GetAnimation())
		||
		(!mScanLineGFX2.gfxentity->GetSurface() && !mScanLineGFX2.gfxentity->GetAnimation())
		||
		(!mScanLineGFX3.gfxentity->GetSurface() && !mScanLineGFX3.gfxentity->GetAnimation())
		||
		(!mScanLineGFX4.gfxentity->GetSurface() && !mScanLineGFX4.gfxentity->GetAnimation())
		||
		(mScanText.gfxentity->GetSurface() || mScanText.gfxentity->GetAnimation())
		)
		throw GenericException("Error while loading AntiVirus element in level config file, Element needs 4 sprites with animation or image and 1 sprite with font!",GenericException::FILE_CONFIG_INCORRECT);

	_resetVariables();
}
	
//Event handling
bool AntiVirusLogic::HandleEvents(const EventData& theevent)
{
	bool eventprocessed(false);

	//IF - Target position changed
	if(theevent.GetEventType() == Event_AgentPosition)
	{
		const AgentPositionEvent& eventdata (static_cast<const AgentPositionEvent&>(theevent));
		if(eventdata.GetAgentId() == mTargetId)
		{
			mTargetPosition = eventdata.GetPosition();
		}
	}//IF - New target received
	else if (theevent.GetEventType() ==  Event_NewAntiVirusTarget)
	{
		const NewAntiVirusTargetEvent& eventdata (static_cast<const NewAntiVirusTargetEvent&>(theevent));
		mTargetId = eventdata.GetAgentId();
		mTargetPosition = eventdata.GetPosition();
		mTrackingPlayer = eventdata.IsPlayer();
		mCounter = mScanTime;
		mTarget = true;
	}//ELSE - New virus created!
	else if (theevent.GetEventType() == Event_NewAgentCreated)
	{
		const NewAgentCreatedEvent& eventdata (static_cast<const NewAgentCreatedEvent&>(theevent));
		
		//IF - It is a virus and we are tracking the player
		if(mTrackingPlayer
		   && 
		   eventdata.GetAgentType() == AI
		   && 
		   eventdata.GetAgentAI() == "SpawnVirus")
		{
			//Fuck! Go to next virus!
			mTarget = false;
		}
	}
	return eventprocessed;
}


//Release resources
void AntiVirusLogic::_release()
{
	//Release all GFX resources
	SingletonIndieLib::Instance()->Entity2dManager->Delete(mScanLineGFX1.gfxentity.get());
	SingletonIndieLib::Instance()->Entity2dManager->Delete(mScanLineGFX2.gfxentity.get());
	SingletonIndieLib::Instance()->Entity2dManager->Delete(mScanLineGFX3.gfxentity.get());
	SingletonIndieLib::Instance()->Entity2dManager->Delete(mScanLineGFX4.gfxentity.get());
	SingletonIndieLib::Instance()->Entity2dManager->Delete(mScanText.gfxentity.get());

}

void AntiVirusLogic::_resetVariables()
{
	mTarget = false;
	mTrackingPlayer = false;
	mCounter = mScanTime;
	mTargetPosition = mScanPosition;
	mTargetId = "";
}