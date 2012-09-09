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
/*TODO:
	- Merge both input controllers classes by creating an interface class, deriving both gamemouse and gamekeyboard
	  from it... but to make it work hastily is good as-is. (no 2 players expected, neither change of controller so..)
	- Generally speaking, input should be decoupled from which library is used. Throughout the game code it is not 
	  done as I dont expect to change things and libraries in short time. but... it is TODO.
*/


#ifndef _GAMEMOUSE
#define _GAMEMOUSE

//Library dependencies

//Class dependencies
#include "Vector2.h"
#include "Shared_Resources.h"

//Forward declarations
class PhysicsSim;
class GameOverlay;

class GameMouse
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameMouse(GameOverlay* overlayptr):
	  mOverlay(overlayptr),
	  mDragging(false),
	  mPositionPix(0.0f,0.0f),
	  mResX(800),
	  mResY(600)
	{	
		_init();
	}
	virtual ~GameMouse()
	{
	}

	//----- GET/SET FUNCTIONS -----
	Vector2 GetPositionPix() { return mPositionPix; }
	//----- OTHER FUNCTIONS -----
	void Update(float dt);		//Update method
	void Render();				//Render necessary elements
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	bool mDragging;				//Tracks if it is dragging some object
	Vector2 mPositionPix;		//Position of mouse cursor in "pixels" units

	GameOverlay* mOverlay;		//Overlay pointer
	float mResX;
	float mResY;
	//----- INTERNAL FUNCTIONS -----
	void _init();
};

#endif