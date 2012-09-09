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

#include "SpriteBuilder.h"
#include "IndieLibManager.h"
#include "ResourceManager.h"
#include "ConfigOptions.h"

//Config options declaration
extern ConfigOptions g_ConfigOptions;

//Definition of static elements
int SpriteBuilder::mSpritesBuilt = 0;

//Read a part of xml info and create a blob object
void SpriteBuilder::ReadSpriteParams(const ticpp::Element* xmlelement) 
{
	/*XML DEFINITION EXPECTED FOR SPRITE ELEMENT:
	Element: SPRITE Atts: Id (string) x(number) y(number) h(number) w(number) Transp(number) FlipX(number) FlipY(number) Wrap(number) WrapX(number) WrapY(number) Layer(number)
						  [OffsetX(number) OffsetY(number) OffsetRot(number)]
					 [Element: Image Atts: Id(string) [CollisionPath(string)]]
					 [Element: Font Atts: Id(string) [LineSpacing(number)] [CharSpacing(number)] [Align(string)] Text(string)]
					 [Element: Animation Atts: Id(string)] 
	*/
	//-------Get attributes from entity---------
	
	//Id
	mSpriteParams.id = xmlelement->GetAttribute("Id");
	
	xmlelement->GetAttribute("x",&mSpriteParams.x);
	xmlelement->GetAttribute("y",&mSpriteParams.y);
	xmlelement->GetAttribute("z",&mSpriteParams.z);
	xmlelement->GetAttribute("h",&mSpriteParams.h);
	xmlelement->GetAttribute("w",&mSpriteParams.w);
	xmlelement->GetAttribute("Rotation",&mSpriteParams.rotation);
	xmlelement->GetAttribute("Transp",&mSpriteParams.transparency);
	xmlelement->GetAttribute("FlipX",&mSpriteParams.flipx);
	xmlelement->GetAttribute("FlipY",&mSpriteParams.flipy);
	xmlelement->GetAttribute("Wrap",&mSpriteParams.wrap); 
	xmlelement->GetAttribute("WrapX",&mSpriteParams.wrapx);
	xmlelement->GetAttribute("WrapY",&mSpriteParams.wrapy);
	xmlelement->GetAttribute("Layer",&mSpriteParams.layer);
	xmlelement->GetAttribute("OffsetX",&mSpriteParams.offsetx,false);  //Optional parameter
	xmlelement->GetAttribute("OffsetY",&mSpriteParams.offsety,false);  //Optional parameter
	xmlelement->GetAttribute("OffsetRot",&mSpriteParams.offsetrot,false);  //Optional parameter

	if(mSpriteParams.h<=0 || mSpriteParams.w<=0 || mSpriteParams.layer<0 || mSpriteParams.layer > 63 || mSpriteParams.transparency < 0 || mSpriteParams.transparency > 255)
		throw GenericException("Element:" + mSpriteParams.id  + "Bad attributes detected! - Out of bounds values",GenericException::FILE_CONFIG_INCORRECT);

	//------Get elements associated to entity-------
	//Only one element can be associated to an entity: Image,Font,Animation
	bool isimage,isfont,isanimation;
	isimage = isfont = isanimation = false;
	ticpp::Iterator <ticpp::Element> entelement;
	//LOOP - Get elements of entity
	for(entelement = entelement.begin(xmlelement);entelement != entelement.end();entelement++)
	{
		//Check what is found
		//IF - FOUND IMAGE
		if(entelement->Value() == "Image" && !isimage && !isfont && !isanimation)
		{
			//Id
			std::string id = entelement->GetAttribute("Id");
			SurfacePointer resource = SingletonResourceMgr::Instance()->GetSurfaceResource(id);
			if(
				id == ""  //Bad id
			    || 
				resource== NULL //Resource not loaded or not found
			 )
			 throw GenericException("Element:" + mSpriteParams.id + "Bad Id '"+ id + "' for element",GenericException::FILE_CONFIG_INCORRECT);
			
			//Store params
			mSpriteParams.resourceid = id;
			mSpriteParams.resourcetype = IMAGE;
			mSpriteParams.collpath = entelement->GetAttribute("CollisionPath");  //OPTIONAL
			//Remember an image was added
			isimage = true;
		}//ELSE IF - FOUND FONT
		else if(entelement->Value() == "Font" && !isimage && !isfont && !isanimation)
		{
			//Id
			std::string id = entelement->GetAttribute("Id");
			FontPointer resource = SingletonResourceMgr::Instance()->GetFontResource(id);
			if(
				id == "" //Bad id
			    ||
			    resource== NULL //Resource not loaded or not found
			 )
			 throw GenericException("Element:" + mSpriteParams.id + "Bad Id '"+ id + "' for subelement",GenericException::FILE_CONFIG_INCORRECT);

			//Store params
			mSpriteParams.resourceid = id;
			mSpriteParams.resourcetype = FONT;

			//Get attributes
			mSpriteParams.align = entelement->GetAttribute("Align"); 
			entelement->GetAttribute("LineSpacing",&mSpriteParams.linespacing); 
			entelement->GetAttribute("CharSpacing",&mSpriteParams.charspacing);		
			if(mSpriteParams.linespacing<0 || mSpriteParams.charspacing<0 || mSpriteParams.align == "")
						throw GenericException("Element:" + mSpriteParams.id + " Bad attributes for subelement '" + id + "'",GenericException::FILE_CONFIG_INCORRECT);
			isfont = true;
		}//ELSE IF - FOUND ANIMATION
		else if(entelement->Value() == "Animation" && !isimage && !isfont && !isanimation)
		{
			//Id
			std::string id = entelement->GetAttribute("Id");
			AnimationPointer resource = SingletonResourceMgr::Instance()->GetAnimationResource(id);
			if(
				id == "" //Bad id
			    ||
			    resource== NULL //Resource not loaded or not found
			 )
			 throw GenericException("Element:" + mSpriteParams.id + "Bad Id '"+ id + "' for subelement",GenericException::FILE_CONFIG_INCORRECT);

			//Store Params
			mSpriteParams.resourceid = id;
			mSpriteParams.resourcetype = ANIMATION;

			isanimation = true;
		}//IF
	}//LOOP END

	mParamsLoaded = true;
}

//Create a new sprite 
void SpriteBuilder::CreateNewSprite()
{
	assert(mParamsLoaded);

	if(!mParamsLoaded)
		return;

	//Get global scale (needed allways)
	float globalscale = SingletonIndieLib::Instance()->GetGeneralScale();
	assert (globalscale > 0.01);

	//Create entity
	mSprite.gfxentity = SpritePointer(new IND_Entity2d());
	mSprite.posoffset = Vector2(mSpriteParams.offsetx,mSpriteParams.offsety);
	mSprite.rotoffset = mSpriteParams.offsetrot;

	//Add resource
	switch(mSpriteParams.resourcetype)
	{
	case (IMAGE):
		{
			//Get surface resource
			SurfacePointer resource = SingletonResourceMgr::Instance()->GetSurfaceResource(mSpriteParams.resourceid);
					
			//Set Surface requested
			mSprite.gfxentity->SetSurface(resource.get());

			//IF - Optional parameter: check it was entered
			if(mSpriteParams.collpath != "")
			{

				std::string fullcollpath = g_ConfigOptions.GetWorkingPath() + mSpriteParams.collpath;
															  //std::String problems with INDIELIB
				if(!mSprite.gfxentity->SetBoundingAreas(const_cast<char*>(fullcollpath.c_str())))
						throw GenericException("Element: " + mSpriteParams.id + " Collision File '" + fullcollpath +"' was not read correctly" ,GenericException::FILE_CONFIG_INCORRECT);
			}
			else //ELSE - Put all sprite bounding area
			{
				if(!mSprite.gfxentity->SetBoundingRectangle ("FullSurface", 0, 0, resource->GetWidth(), resource->GetHeight()))
						throw GenericException("Element:" + mSpriteParams.id + "Could not assign default bounding collision area for sprite - Check if element is too smal...",GenericException::LIBRARY_ERROR);
			}//IF	
		
			mSprite.gfxentity->SetHotSpot(0.5f,0.5f);
			Vector2 positionpix = SingletonIndieLib::Instance()->FromCoordToPix(Vector2(mSpriteParams.x,mSpriteParams.y));
 			mSprite.gfxentity->SetPosition(static_cast<float>(positionpix.x),static_cast<float>(positionpix.y),mSpriteParams.z);//Position
			SingletonIndieLib::Instance()->ScaleToFit(mSprite.gfxentity.get(),
						resource.get(),
						static_cast<int>(mSpriteParams.w*globalscale),
						static_cast<int>(mSpriteParams.h*globalscale));//Scale to fit dimensions			
		}
		break;
	case (FONT):
		{
			//Get resource
			FontPointer resource = SingletonResourceMgr::Instance()->GetFontResource(mSpriteParams.resourceid);

			//Set font to entity
			mSprite.gfxentity->SetFont(resource.get());

			//Set align
			if( mSpriteParams.align == "Left")
				mSprite.gfxentity->SetAlign(IND_LEFT);
			else if(mSpriteParams.align == "Center")
				mSprite.gfxentity->SetAlign(IND_CENTER);
			else if(mSpriteParams.align == "Right")
				mSprite.gfxentity->SetAlign(IND_RIGHT);
			
			//Set spacings
			mSprite.gfxentity->SetCharSpacing(mSpriteParams.charspacing);
			mSprite.gfxentity->SetLineSpacing(mSpriteParams.linespacing);
			
			//Set general parameters associated to entity (after assigning surface/animation/font/whatever)				
			mSprite.gfxentity->SetHotSpot(0.0f,0.0f);
			Vector2 positionpix = SingletonIndieLib::Instance()->FromCoordToPix(Vector2(mSpriteParams.x,mSpriteParams.y));
 			mSprite.gfxentity->SetPosition(static_cast<float>(positionpix.x),static_cast<float>(positionpix.y),mSpriteParams.z);//Position
			
		}
		break;
	case (ANIMATION):
		{
			//Get Resource
			AnimationPointer resource = SingletonResourceMgr::Instance()->GetAnimationResource(mSpriteParams.resourceid);

			//Set animation to entity
			mSprite.gfxentity->SetAnimation(resource.get());
	
			//Set other params
			mSprite.gfxentity->SetHotSpot(0.5f,0.5f);
			Vector2 positionpix = SingletonIndieLib::Instance()->FromCoordToPix(Vector2(mSpriteParams.x,mSpriteParams.y));
 			mSprite.gfxentity->SetPosition(static_cast<float>(positionpix.x),static_cast<float>(positionpix.y),mSpriteParams.z);//Position
			//Scale to fit (as it is animation, scale to bigger animation...
			SingletonIndieLib::Instance()->ScaleToFit(mSprite.gfxentity.get(),
						resource.get(),
						static_cast<int>(mSpriteParams.w*globalscale),
						static_cast<int>(mSpriteParams.h*globalscale));//Scale to fit dimensions
		}
		break;
	default:
		{
			//NEVER!!!
			assert(false);
		}
		break;
	}

	//Additional settings - Dont show additionals, set by sprite if needed
	mSprite.gfxentity->ShowGridAreas(false); 
	mSprite.gfxentity->ShowCollisionAreas(false);

	//Rotation
	mSprite.gfxentity->SetAngleXYZ(0.0f,0.0f,mSpriteParams.rotation);

	//Transparency and image flipping
	mSprite.gfxentity->SetTransparency(static_cast<byte>(mSpriteParams.transparency));
	mSprite.gfxentity->SetMirrorX(mSpriteParams.flipx);
	mSprite.gfxentity->SetMirrorY(mSpriteParams.flipy);
	mSprite.gfxentity->ToggleWrap(mSpriteParams.wrap);
	if(mSpriteParams.wrap)
	{
		mSprite.gfxentity->SetRegion(0,0,mSpriteParams.wrapx,mSpriteParams.wrapy);
	}

	//When finalized creation, add to IndieLib
	SingletonIndieLib::Instance()->Entity2dManager->Add(mSpriteParams.layer,mSprite.gfxentity.get());
	
	++mSpritesBuilt;
	SingletonLogMgr::Instance()->AddNewLine("SpriteBuilder::CreateNewSprite","Entity '" + mSpriteParams.id + "' created",LOGDEBUG);
}

//Save parameters of sprite in an xml element
void SpriteBuilder::SaveSprite(SpritePointer thesprite, ticpp::Element& xmlelement)
{
	/*XML DEFINITION EXPECTED FOR SPRITE ELEMENT:
	Element: SPRITE Atts: Id (string) x(number) y(number) h(number) w(number) Layer(number)
					 [Element: Image Atts: Id(string) [CollisionPath(string)]]
					 [Element: Font Atts: Id(string) [LineSpacing(number)] [CharSpacing(number)] [Align(string)] Text(string)]
					 [Element: Animation Atts: Id(string)] 
	*/
	
	//Get needed values from GFX system for calculations
	float globalscale = SingletonIndieLib::Instance()->GetGeneralScale();

	//-------Get attributes to save from entity---------
	float h,w; //Later is initialized
	float x = thesprite->GetPosX();
	float y = thesprite->GetPosY();
	Vector2 coordpos = SingletonIndieLib::Instance()->FromPixToCoord(Vector2(x,y));
	int z = thesprite->GetPosZ();
	float rotation = thesprite->GetAngleZ();
	int transparency = thesprite->GetTransparency();
	bool mirrorx = thesprite->GetMirrorX();
	bool mirrory = thesprite->GetMirrorY();
	bool wrap = thesprite->IfWrap();
	int wrapx = thesprite->GetRegionWidth();
	int wrapy = thesprite->GetRegionHeight();
	int layer = thesprite->GetLayer();

	//----Get elements associated with entity and remaining attributes------
	//Only one element can be associated to an entity: Image,Font,Animation
	//IF - Sprite is an image
	if(thesprite->GetSurface() != NULL)
	{
		IND_Surface* thesurface = thesprite->GetSurface();
		//Get Id of related surface
		std::string surfaceid = SingletonResourceMgr::Instance()->GetSurfaceId(thesurface);
		//Create "Image" element inside sprite
		ticpp::Element imageelement("Image");
		imageelement.SetAttribute("Id",surfaceid);
		xmlelement.InsertEndChild(imageelement);

		//Set remaining attributes from sprite
		w = (thesurface->GetWidth() * thesprite->GetScaleX()) / globalscale; 
		h = (thesurface->GetHeight() * thesprite->GetScaleY()) / globalscale;
	}//ELSE IF - Sprite is an animation
	else if(thesprite->GetAnimation() != NULL)
	{
		IND_Animation* theanimation = thesprite->GetAnimation();
		//Get Id of related animation
		std::string animationid = SingletonResourceMgr::Instance()->GetAnimationId(theanimation);
		//Create "Animation" element inside sprite
		ticpp::Element animationelement("Animation");
		animationelement.SetAttribute("Id",animationid);
		xmlelement.InsertEndChild(animationelement);

		//Find higher Width and Height in all sequences to calculate h and w
		int numsequences = theanimation->GetNumSequences();
		int hightwidth, highheight; 
		hightwidth = highheight = 0;
		//LOOP  Find bigger x and y to scale
		for(int i = 0;i<numsequences;i++)
		{
			int animwidth = theanimation->GetHighWidth(i);  //Bigger width of this sequence
			int animheight = theanimation->GetHighHeight(i); //Bigger heigth of this sequence

			//Assignment if it is bigger
			if(animwidth > hightwidth)
				hightwidth =  animwidth;
			if(animheight > highheight)
				highheight =  animheight;
		}//LOOP END

		//Set remaining attributes from sprite
		w = (hightwidth * thesprite->GetScaleX()) / globalscale; 
		h = (highheight * thesprite->GetScaleY()) / globalscale;
	}
	//TODO: SAVE FONT SETTINGS
	/*else if(////CODE \\\\\ IS A FONT)
	{

	}*/

	//Set all attributes of entity in order (to ease reading of file)
	xmlelement.SetAttribute("x",coordpos.x);
	xmlelement.SetAttribute("y",coordpos.y);
	xmlelement.SetAttribute("z",z);
	xmlelement.SetAttribute("w",w);
	xmlelement.SetAttribute("h",h);
	xmlelement.SetAttribute("Rotation",rotation);
	xmlelement.SetAttribute("Transp",transparency);
	xmlelement.SetAttribute("FlipX",mirrorx);
	xmlelement.SetAttribute("FlipY",mirrory);
	xmlelement.SetAttribute("Wrap",wrap);
	xmlelement.SetAttribute("WrapX",wrapx);
	xmlelement.SetAttribute("WrapY",wrapy);
	xmlelement.SetAttribute("Layer",layer);

}