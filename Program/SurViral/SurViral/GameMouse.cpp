/*
	Filename: GameMouse.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to mouse input in game screen
	Comments: It depends of IndieLib, as it will be library used for input
	          The game mouse, can take objects and move them using the physics engine
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "GameMouse.h"
#include "IndieLibManager.h"
#include "GameOverlay.h"
#include "SVGame.h"
#include "PhysicsManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"
#include "Camera2D.h"

//Update method
void GameMouse::Update(float)
{
	//Get input and physics manager
	IND_Input* input = SingletonIndieLib::Instance()->Input;
	
#ifdef _DEBUGGING
	if(input->OnMouseButtonPress(IND_MBUTTON_WHEELUP ))
	{
		Camera2DPointer camera = SingletonIndieLib::Instance()->GetCamera("General");
		camera->Zoom(true);
	}

	if(input->OnMouseButtonPress(IND_MBUTTON_WHEELDOWN ))
	{
		Camera2DPointer  camera = SingletonIndieLib::Instance()->GetCamera("General");
		camera->Zoom(false);
	}
#endif

	//Update mouse position
	mPositionPix.x = static_cast<float>(input->GetMouseX());
	mPositionPix.y = static_cast<float>(input->GetMouseY());

	//Send event with new position
	NewMousePosInfo newposinfo(mPositionPix);

	SingletonGameEventMgr::Instance()->QueueEvent(
		                             EventDataPointer(new NewMousePosEvent(Event_NewMousePos,newposinfo))
										 );

	//Shoot command
	if(input->OnMouseButtonPress(IND_MBUTTON_LEFT))
	{
		//Send event with new position
		Vector2 reltocenterpos (mPositionPix.x - mResX/2, mResY/2 - mPositionPix.y );
		reltocenterpos.Normalise();
		ShootCommandInfo commandinfo(reltocenterpos);

		SingletonGameEventMgr::Instance()->QueueEvent(
										 EventDataPointer(new ShootCommandEvent(Event_ShootCommand,commandinfo))
											 );		
	}
}
	
//Render necessary elements
void GameMouse::Render()
{
	//Render mouse is made by entities...
}

void GameMouse::_init()
{
	mResX = static_cast<float>(SingletonIndieLib::Instance()->Window->GetWidth());
	mResY = static_cast<float>(SingletonIndieLib::Instance()->Window->GetHeight());
}