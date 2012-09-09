/*
	Filename: SpriteBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric sprite of IndieLib from XML file
				 It also saves to xml file the parameters of sprite
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _SPRITEBUILDER
#define _SPRITEBUILDER

//Library dependencies	
#include "boost\shared_ptr.hpp"  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...

#include <string>
//Class dependencies
#include "XMLParser.h"
#include "LogManager.h"
#include "GenericException.h"
#include "GFXDefs.h"


class SpriteBuilder
{
private:
	//Creation parameters for sprite
	enum SpriteResourceType {IMAGE,ANIMATION,FONT};
	typedef struct GFXEntityParams{
		GFXEntityParams():
        offsetx(0.0f),offsety(0.0f),offsetrot(0.0f),collpath("")
		{}
		std::string id;
		float x,y,h,w;
		int z;
		float rotation;
		int transparency;
		bool flipx, flipy,wrap;
		int wrapx,wrapy;
		int layer;
		float offsetx,offsety,offsetrot;
		std::string resourceid;
		SpriteResourceType resourcetype;
		std::string collpath;   //IMG-only params
		std::string align;  //FONT-only params
		int linespacing,charspacing;  //FONT-only params
	} GFXEntityParams;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SpriteBuilder():
	mParamsLoaded(false)
	{
	}
	~SpriteBuilder()
	{
	}
	//----- GET/SET FUNCTIONS -----
	ContainedSprite GetCreatedSprite() { assert (mParamsLoaded); return mSprite; }  //Returns A COPY of the created sprite
	//----- OTHER FUNCTIONS -----
	void ReadSpriteParams(const ticpp::Element* xmlelement);   //Read a part of xml info without creating a new sprite
	void CreateNewSprite();   //Create a new sprite
	void SaveSprite(const SpritePointer thesprite, ticpp::Element& xmlelement); //Save parameters of sprite in an xml element
protected:
	//----- INTERNAL VARIABLES -----
	ContainedSprite mSprite;
	GFXEntityParams mSpriteParams;
	bool mParamsLoaded;

	static int mSpritesBuilt;
	//----- INTERNAL FUNCTIONS -----
};

#endif