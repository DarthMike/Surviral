/*
	Filename: GameKeyBoard.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to keyboard input
	Comments: It depends of IndieLib, as it will be library used for input
	          The keyboard is used to control things by the user. In this game, it controls the main character,
			  a squishy blob of water! ;)
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "GameKeyBoard.h"
#include "GameOverlay.h"
#include "Camera2D.h"
#include "IndieLibManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"

//Update method
void GameKeyBoard::Update(float)
{
	//Get input and process
	IND_Input* input = SingletonIndieLib::Instance()->Input;
	
	//Reset movement direction command
	mMoveDirection = Vector2(0,0);

	//Check inputs
	if(input->IsKeyPressed(IND_KEYLEFT) && !input->IsKeyPressed(IND_KEYRIGHT))
	{
		mMoveDirection.x -= 1;
	}
	else if(!input->IsKeyPressed(IND_KEYLEFT) && input->IsKeyPressed(IND_KEYRIGHT))
	{
		mMoveDirection.x += 1;
	}

	if(input->IsKeyPressed(IND_KEYUP) && !input->IsKeyPressed(IND_KEYDOWN))
	{
		mMoveDirection.y += 1;
	}
	else if(!input->IsKeyPressed(IND_KEYUP) && input->IsKeyPressed(IND_KEYDOWN))
	{
		mMoveDirection.y -= 1;
	}

	//Construct and send event
	MoveCommandInfo data(mMoveDirection);
	SingletonGameEventMgr::Instance()->QueueEvent(
	EventDataPointer (new MoveCommandEvent(Event_MoveCommand,data))
						  );
	

	//--------Exit-----------
	if(input->IsKeyPressed(IND_ESCAPE))
	{
		//Send event of "exit"
		SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new EventData(Event_ExitGame))
											);
	}
	//DEBUG MODE ONLY FOR DEVELOPING
#ifdef _DEBUGGING
	static bool toggle(true);
	if(input->OnKeyPress(IND_F1))
	{
		SingletonIndieLib::Instance()->SetDebugDraw(toggle);
		toggle = !toggle;
	}
#endif
}
	
//Render necessary elements
void GameKeyBoard::Render()
{
	//For now done by entities
}