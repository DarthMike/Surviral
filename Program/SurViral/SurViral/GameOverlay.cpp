/*
	Filename: GameOverlay.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to overlay presentation (UI)
	Comments: It depends of IndieLib
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/		

#include "GameOverlay.h"
#include "GameOverlayListener.h"
#include "Math.h"
#include "GameMouse.h"
#include "GameKeyboard.h"
#include "ConfigOptions.h"
#include "GameEventManager.h"
#include "IndieLibManager.h"
#include "GFXEffects.h"
#include "GameLevel.h"
#include "LevelBuilder.h"
#include "GameEvents.h"
#include "Camera2D.h"
#include "SoundManager.h"
#include "ResourceManager.h"

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Definition of entities file to editor
const std::string GameOverlay::mOverlayAssetsFileName = "OverlayAssets.xml";

//Update method
void GameOverlay::Update(float dt)
{
	static bool firstplay (true);
	//Local variables
	IndieLibManager* Ilib (SingletonIndieLib::Instance());
	GFXEffects* effecsmgr (SingletonIndieLib::Instance()->GFXEffectsManager);

	//Update elements of overlay
	//Update input controllers
	mGameMouse->Update(dt);
	mGameKeyBoard->Update(dt);
	
	//Update internal timers
	mCounter += dt;

	//Update mouse cursor position
	_updateMouseGFX();

	//Update created dynamic animations
	std::list<AnimationControllerPointer>::iterator anitr = mDynamicAnimations.begin();
	//LOOP - For each element created
	while(anitr != mDynamicAnimations.end())
	{
		//Update controller
		(*anitr)->Update(dt);
		if((*anitr)->IsCurrentAnimationFinished())	
		{
			anitr = mDynamicAnimations.erase(anitr);
		}
		else
		{
			++anitr;
		}

	}//LOOP END

	//Update created dynamic sprites
	std::list<SpritePointer>::iterator spitr = mDynamicSprites.begin();
	//LOOP - For each element created
	while(spitr != mDynamicSprites.end())
	{
		if(effecsmgr->IsEffectComplete((*spitr)))
		{
			//Delete it from indielib and destroy it
			Ilib->Entity2dManager->Delete((*spitr).get());
			spitr = mDynamicSprites.erase(spitr);
		}
		else
		{
			++spitr;
		}

	}//LOOP END
	
	//--------First play only elements---------------------
	if(firstplay)
	{
		firstplay = false;
	}
	
	//Debug text is updated by messages
	//Messages text is updated by events

	//---Game start---
	//IF - Game wasnt started
	if(!mGameStarted)
	{
		static bool introshow(false);
		
		//INTRO SHOWING
		if(!introshow)
		{
			SingletonSoundMgr::Instance()->StopMusic("LevelComplete",true);
			SingletonSoundMgr::Instance()->PlayAction("LevelStart");
			mOverlayAssets->GetEntity("StartMSG")->SetShow(true);
			introshow = true;
		}
		//IF - User presses input or elapsed time
		if(Ilib->Input->OnKeyPress(IND_SPACE)
		   ||
		   Ilib->Input->OnKeyPress(IND_RETURN)
		   ||
		   Ilib->Input->OnMouseButtonPress(IND_MBUTTON_LEFT)
		   ||
		   Ilib->Input->OnMouseButtonPress(IND_MBUTTON_RIGHT)
		   )
		{
			mOverlayAssets->GetEntity("StartMSG")->SetShow(false);
			
			SingletonSoundMgr::Instance()->PlayMusic("LevelMusic");

			//Send message to start game!
			SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new EventData(Event_GameStart))
														  );
			mGameStarted = true;
			introshow = false;
		}//IF
	}//IF

	//---Game Over--
	//IF - Game over
	if(mGameOver)
	{
		//Local control variable
		static bool processed(false);
		//Display message to pass to next level
		//IF - Processed text display for next level
		if(!processed)
		{
			//Show corresponding message
			SpritePointer messagetext = mOverlayAssets->GetEntity("MessagesText");
			mMessagesText = "...SYSTEM CLEAN...\n\nPress SPACE to infect again\nESC to exit";
			messagetext->SetText(const_cast<char*>(mMessagesText.c_str()));
			SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(messagetext,1.0f,Vector3(0,0.5,0));
			processed = true;
			//Stop Music
			SingletonSoundMgr::Instance()->StopMusic("LevelMusic",true);
			SingletonSoundMgr::Instance()->StopMusic("Scanning",true);
		}
		else if(processed)
		{
			//IF - Restart
			if(Ilib->Input->OnKeyPress(IND_SPACE))
			{
				//Send message to restart level
				SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new EventData(Event_RestartLevel))
											);
				_resetVariables();
				processed = false;
			}//IF	
		}//IF
	}//IF
		
	//---Level completed--
	//IF - Level completed
	if(mLevelCompleted)
	{
		//Local control variable
		static bool processed(false);
		//Display message to pass to next level
		//IF - Processed text display for next level
		if(!processed)
		{
			//Show corresponding message
			SpritePointer messagetext = mOverlayAssets->GetEntity("MessagesText");
			mMessagesText = "...::ERROR::...\n...SYSTEM INFECTED...\n\nPress SPACE to SPREAD infection\nESC to quit";
			messagetext->SetText(const_cast<char*>(mMessagesText.c_str()));
			SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(messagetext,0.5f,Vector3(0,0,0));
			processed = true;
			//Stop Music
			SingletonSoundMgr::Instance()->StopMusic("LevelMusic",true);
			SingletonSoundMgr::Instance()->StopMusic("Scanning",true);
			//Play level complete music
			SingletonSoundMgr::Instance()->PlayMusic("LevelComplete",false);
		}
		else if(processed)
		{
			//IF - Continue
			if(Ilib->Input->OnKeyPress(IND_SPACE))
			{
				//Send message to load next level
				SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new EventData(Event_NextLevel))
											);
				_resetVariables();
				processed = false;
			}//IF
		}//IF
	}//IF*/
		
}

//Render necessary elements
void GameOverlay::Render()	
{
	mGameMouse->Render();
	mGameKeyBoard->Render();
}

//Event handling
bool GameOverlay::_handleEvents(const EventData& theevent)
{
	//Local variables
	bool eventprocessed (false);

	//Receiving of events:
	//Check type of event
	//IF - "Exit game" event
	if(theevent.GetEventType() == Event_ExitGame)
	{
		//Just stop music
		SingletonSoundMgr::Instance()->StopMusic("LevelMusic",true);
	}//ELSE - Game Start//ELSE - Game Over
	else if(theevent.GetEventType() == Event_GameOver)
	{
		mGameOver = true;

	}//ELSE - Level complete
	else if(theevent.GetEventType() == Event_LevelCompleted)
	{
		mLevelCompleted = true;		
	}
	else if(theevent.GetEventType() == Event_DebugString)
	{
		const DebugMessageEvent& data = static_cast<const DebugMessageEvent&>(theevent);
		static int linenum = 0;
		if(mDebugLines <= 5)
		{
			mDebugLines += 1;
		}
		else
		{
			mDebugMSGstream.seekp(0);
			mDebugMSGstream.seekg(0);
			mDebugMSGstream.clear();
			mDebugLines = 0;
		}
		linenum++;
		std::string newmsg = data.GetString();;
		mDebugMSGstream<<"\n"<<linenum<<":";
		mDebugMSGstream<<newmsg;
		//Just display the messages
		mDebugText = mDebugMSGstream.str();
		SpritePointer debugtext = mOverlayAssets->GetEntity("DebugText");
		debugtext->SetText(const_cast<char*>(mDebugText.c_str()));
		eventprocessed = true;
	}//ELSE - Game statistics update
	else if(theevent.GetEventType() == Event_GameStatistics)
	{
		const GameStatisticsEvent& eventdata (static_cast<const GameStatisticsEvent&>(theevent));
		const GameStatisticsInfo& eventinfo (eventdata.GetInfo());
		
		//Update display of values
		mMinutes = eventinfo.minutes;
		mSeconds = eventinfo.seconds;

		std::stringstream message;
		message<<"INFECT TIME - "<<mMinutes<<" m :"<<mSeconds<<" s"
			   <<"\nANTIVIRUS SCAN COMPLETE IN:"
			   <<"\n"<<eventinfo.scanseconds<<" s";
			   //<<"\n"<<eventinfo.scanmilliseconds<<" ms";
		//Store text
		mStatisticsText = message.str();
		mOverlayAssets->GetEntity("StatisticsText")->SetText(const_cast<char*>(mStatisticsText.c_str()));   //Indielib incompatibility with string! (const_cast)
		mOverlayAssets->GetEntity("StatisticsText")->SetShow(true);
	}//ELSE - Player position changed
	else if(theevent.GetEventType() == Event_AgentPosition)
	{
		const AgentPositionEvent& eventdata (static_cast<const AgentPositionEvent&>(theevent));
		if(eventdata.GetAgentType() == PLAYER)
		{
			SingletonIndieLib::Instance()->GetCamera("General")->MoveTo(eventdata.GetPosition());
		}
	}//ELSE - New agent created
	else if(theevent.GetEventType() == Event_NewAgentCreated)
	{
		const NewAgentCreatedEvent& eventdata(static_cast<const NewAgentCreatedEvent&>(theevent));
		
		//Get which type of agent was created
		//New projectile
		if(eventdata.GetAgentType() == PROJECTILE)
		{
			SingletonSoundMgr::Instance()->PlayAction("Blip");	
		}
		else if(eventdata.GetAgentType() == AI && eventdata.GetAgentAI() == "SpawnVirus")
		{
			SingletonSoundMgr::Instance()->PlayAction("NewVirus");	
		}
	}//ELSE - Solid Collision
	else if(theevent.GetEventType() == Event_SolidCollision)
	{
		const SolidCollisionEvent& eventdata(static_cast<const SolidCollisionEvent&>(theevent));
		
		const SolidCollisionInfo& eventinfo(eventdata.GetInfo());
		switch(eventinfo.material)
		{
		case (DATACHUNK):
			{
				SingletonSoundMgr::Instance()->PlayAction("BlipBounce");
			}
			break;
		case (CRITICALSECTION):
			{
				SingletonSoundMgr::Instance()->PlayAction("BlipBounce2");
			}
			break;
		case (GENERIC):
			{
				SingletonSoundMgr::Instance()->PlayAction("BlipBounce3");
			}
			break;
		}
	}//ELSE - New target for antivirus
	else if(theevent.GetEventType() == Event_NewAntiVirusTarget)
	{
		SingletonSoundMgr::Instance()->StopMusic("Scanning",true);
		SingletonSoundMgr::Instance()->PlayMusic("Scanning",false);
	}//ELSE - Agent Destroyed
	else if (theevent.GetEventType() == Event_AgentDestroyed)
	{
		
		const AgentDestroyedEvent& eventdata(static_cast<const AgentDestroyedEvent&>(theevent));
		//Get which type of agent was destroyed
		if(eventdata.GetAgentType() == AI && eventdata.GetAgentAI() == "SpawnVirus"
		   ||
		   eventdata.GetAgentType() == PLAYER)
		{
			SingletonSoundMgr::Instance()->PlayAction("TerminateVirus");
		}
	}//ELSE - DataBlock Corrupted
	else if(theevent.GetEventType() == Event_DataBlockCorrupted)
	{
		const DataBlockCorruptedEvent& eventdata (static_cast<const DataBlockCorruptedEvent&>(theevent));
		
		//Get position and play animation and sound
		Vector2 position (eventdata.GetPosition());
		
		//Play animation

		//Play sound
		SingletonSoundMgr::Instance()->PlayAction("DataCorrupted");
		
	}

	return (eventprocessed);
}

void GameOverlay::_init()
{
	//**********************LOAD NEEDED ASSETS*************************************
	//Load entities related to this screen
	std::string assetsfilepath = g_ConfigOptions.GetScriptsPath() + mOverlayAssetsFileName;
	
	//---Creation of entities through level builder---
	LevelBuilder thebuilder; //Load level assets - Container mode
			
	//Call creation of object (level)
	thebuilder.LoadLevel(assetsfilepath,"-**NOEDIT**OVERLAYASSETS-",true);  //Only assets loading (last "true")
	
	//No errors, get level pointer (used to store entities assets)
	mOverlayAssets = thebuilder.GetCreatedLevel();
	//**********************************ASSETS LOADED*******************************
	
	//**********************SET INTERNAL VARIABLES**********************************
	mScaleFactor = SingletonIndieLib::Instance()->GetGeneralScale();
	mResX = static_cast<float>(SingletonIndieLib::Instance()->Window->GetWidth());
	mResY = static_cast<float>(SingletonIndieLib::Instance()->Window->GetHeight());

	//Check all entities were loaded
	SpritePointer pt;
	pt = mOverlayAssets->GetEntity("DebugText");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'DebugText'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	#ifdef _DEBUGGING
	pt->SetText("--DEBUG TEXT HERE--");
	#endif

	pt = mOverlayAssets->GetEntity("MessagesText");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'MessagesText'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetTint(0,0,0);

	pt = mOverlayAssets->GetEntity("StatisticsText");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'StatisticsText'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetTint(0,0,0);

	
	pt = mOverlayAssets->GetEntity("MouseCursor");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'MouseCursor'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetShow(true);

	pt = mOverlayAssets->GetEntity("StartMSG");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'StartMSG'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetShow(true);

	_resetVariables();
	//*********************CREATE INPUT CONTROLLERS********************************
	mGameMouse = new GameMouse(this);
	mGameKeyBoard = new GameKeyBoard(this);

	//*****************INPUT CONTROLLERS CREATED***********************************

	//Register game overlay listener
	if(!mListener)
		mListener = new GameOverlayListener(this);
}

void GameOverlay::_resetVariables()
{
	mMessagesText = "";
	mDisplayNextMessages = true;
	mCounter = 0.0f;
	mGameOver = false;
	mLevelCompleted = false;
	mGameStarted = false;

	mMinutes = 0;
	mSeconds = 0;

	SpritePointer msg(mOverlayAssets->GetEntity("MessagesText"));
	msg->SetShow(false);
}

void GameOverlay::_release()
{

	//Deregister from IndieLib any dynamically created sprites (animations is done in animationcontroller)
	std::list<SpritePointer>::iterator itr;
	//LOOP - Deregister remaining sprites
	for(itr = mDynamicSprites.begin(); itr != mDynamicSprites.end(); ++itr)
	{
		SingletonIndieLib::Instance()->GFXEffectsManager->StopEffect((*itr));
		SingletonIndieLib::Instance()->Entity2dManager->Delete((*itr).get());
	}//LOOP END

	//Input controllers
	if(mGameMouse)
	{
		delete mGameMouse;
		mGameMouse = NULL;
	}
	if(mGameKeyBoard)
	{
		delete mGameKeyBoard;
		mGameKeyBoard = NULL; 
	}

	if(mListener)
	{
		delete mListener;
		mListener = NULL;
	}
}

void GameOverlay::_updateMouseGFX()
{
	//-----Put in position of mouse (pixels) to render in overlay camera-----------
	Vector2 positionpix(mGameMouse->GetPositionPix());

	//--------------Update mouse display position---------------------------------
	mOverlayAssets->GetEntity("MouseCursor")->SetPosition(static_cast<float>(positionpix.x - mResX/2),
							   static_cast<float>(positionpix.y + mResY/2),
							   0);
}