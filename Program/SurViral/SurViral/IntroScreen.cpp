/*
	Filename: IntroScreen.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - INTROSCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#include "IntroScreen.h"
#include "LevelBuilder.h"
#include "ConfigOptions.h"
#include "IndieLibManager.h"

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Constants declarations
const std::string IntroScreen::mScreenAssetsFileName = "IntroScreenAssets.xml";

//To execute on start
void IntroScreen::ExecuteEnter()
{
	//Perform a fadein of main text
	SpritePointer tutorial (mScreenAssets->GetEntity("Tutorial"));
	tutorial->SetShow(true);
}
void IntroScreen::ExecuteRender()
{
	//Rendering is done by entities -> IndieLib
}

//To execute while rendering
void IntroScreen::ExecuteLogicUpdate(float dt)
{
	//Increment internal counter
	mCounter += dt;

	//IF - First transition
	if(mCounter > 10000.0f)
	{
		mExit = true;
	}

	//Finally, process inputs from user
	_processInputs();
}

//To execute at exit
void IntroScreen::ExecuteExit()
{
	//Perform a fadein of main text
	SpritePointer tutorial (mScreenAssets->GetEntity("Tutorial"));
	tutorial->SetShow(false);
}

//Process possible events
bool IntroScreen::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}


//Return a game state if it is necessary to exit
GameState IntroScreen::IsExit()
{
	//Check if it is needed to change screen
	if(mExit)
		return (NORMALGAME); //To game 
	else
		return(NOSTATE); //No change
}

//Internal init
void IntroScreen::_init()
{
	//**********************LOAD NEEDED ASSETS*************************************
	//Load entities related to this screen
	std::string assetsfilepath = g_ConfigOptions.GetScriptsPath() + mScreenAssetsFileName;
	
	//---Creation of entities through level builder---
	LevelBuilder thebuilder; //Load level assets - Container mode
			
	//Call creation of object (level)
	thebuilder.LoadLevel(assetsfilepath,"-**NOEDIT**INTROSCREENASSETS**-",true);  //Only assets loading (last "true")
	
	//No errors, get level pointer (used to store entities assets)
	mScreenAssets = thebuilder.GetCreatedLevel();
	//**********************************ASSETS LOADED*******************************

	//Check all entities were loaded
	SpritePointer pt;
	pt = mScreenAssets->GetEntity("Tutorial");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'Tutorial'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}

	//Set initial properties
	pt->SetShow(false);
}
//Internal release
void IntroScreen::_release()
{
	//Dynamically allocated memory...	
	//Release from memory and graphics system is done by base class
}

void IntroScreen::_processInputs()
{
	IndieLibManager* pIlib = SingletonIndieLib::Instance();

	//--------Exit-----------
	if(pIlib->Input->OnKeyPress(IND_ESCAPE)
	   ||
	   pIlib->Input->OnKeyPress(IND_RETURN)
	   ||
	   pIlib->Input->OnKeyPress(IND_MBUTTON_LEFT)
	   ||
	   pIlib->Input->OnKeyPress(IND_MBUTTON_RIGHT))
	{
		mExit = true;
	}
}