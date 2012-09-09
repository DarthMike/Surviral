/*
	Filename: LevelBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric level from an XML file
				 It also saves to xml file the parameters of level
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
#include "LevelBuilder.h"
#include "SVGame.h"
///HACK
#include "AntiVirusLogic.h"
///HACK END
//#include "EditorLogic.h"
#include "ResourceManager.h"
#include "AgentsManager.h"
#include "IndieLibManager.h"
#include "PhysicsManager.h"
#include "SpriteBuilder.h"
#include "SolidBodyAgentBuilder.h"
#include "PlayerBuilder.h"
#include "AIAgentBuilder.h"
#include "Camera2D.h"
#include "GameLogicDefs.h"

//Load a level given the level name
void LevelBuilder::LoadLevel(const std::string& filepath, const std::string& levelname, bool assetsonly)
{

	//***********LOAD ALL LEVEL ENTITIES FROM LEVEL FILE******************
	//Load all entities from file
	/*Expected configuration file
	Element:    - Parallax: Atts: MaxZoom(number), MinZoom(number)
	                    Element: LAYER
				- Entities
						Element: SPRITE 
 						Element: BODY
						Element: JOINT
						Element: PLAYER
						Element: AI
						Element: COLLECTABLE
						.
						.
						.
	*/
	
	assert(!mLevelPointer); //Assertion not to use the same builder and ovewrite level data!!!
/*	assert(
		   ((((mGameLogicptr && !mEditorLogicptr)
		   ||
		   (!mGameLogicptr && mEditorLogicptr)))
		   &&
		   !assetsonly)
		   ||
		   assetsonly
		  );  //Assure only one pointer is used (in-game or editor modes, not the same!)*/

	assert((assetsonly && !mGameLogicptr)
		   ||
		   !assetsonly);  //Only assets mode preceded by good builder constructed*

	//Open and load document
	ticpp::Document configdoc(filepath);
	configdoc.LoadFile();	//Parsing

	//Creation of level object
	mLevelPointer = GameLevelPointer(new GameLevel(levelname));
	
	//Creation of level - Handle exceptions!
	try
	{
		if(!assetsonly)
		{
			//----------Get attributes for level-----------
			//Nothing for now
			//ticpp::Element* levelsection =  configdoc.FirstChildElement("Level");
			
			//----------Parallax layers creation-----------
			//Get Parallax section
			ticpp::Element* parallaxsection =  configdoc.FirstChildElement("Parallax");


			//Query properties of parallax scroll
			float maxzoom, minzoom, startzoom;
			parallaxsection->GetAttribute("MaxZoom",&maxzoom);
			parallaxsection->GetAttribute("MinZoom",&minzoom);
			parallaxsection->GetAttribute("StartZoom",&startzoom);
			//Check correctness
			if(maxzoom < minzoom || startzoom < minzoom || startzoom > maxzoom || maxzoom < 0 || minzoom < 0 || startzoom < 0 )
				throw GenericException("Failure while reading '" + filepath + "'Parameters MaxZoom, MinZoom, StartZoom,  need to be coherent (max>startzoom>min)! in Parallax section",GenericException::FILE_CONFIG_INCORRECT);
			
			//Set general camera properties for this parallax in level
			SingletonIndieLib::Instance()->GetCamera("General")->SetMaxMinZoom(maxzoom,minzoom);
			SingletonIndieLib::Instance()->GetCamera("General")->SetZoom(startzoom);

			//Query for elements in parallax section
			ticpp::Iterator<ticpp::Element> parallaxitr;  //TiCpp iterator
			//LOOP - Get Layers from XML
			for(parallaxitr = parallaxitr.begin(parallaxsection);parallaxitr != parallaxitr.end();parallaxitr++)
			{
				//Get the type of element
				std::string type;
				parallaxitr->GetValue(&type);
				//Process types of elements in section
				if(type == "Layer") //Parallax Layer type
					_processParallaxLayer(parallaxitr,filepath);	

			}//LOOP END	

			//Once layers are created, sort them by layer number
			GameLevel::ParallaxCompareClass comparefcn;
			mLevelPointer->mParallaxList.sort(comparefcn);
		}
		//----------Entities creation-------------------

		//Query for elements in entities sections
		ticpp::Element* entitiessection =  configdoc.FirstChildElement("Entities");
		ticpp::Iterator<ticpp::Element> entsitr;  //TiCpp iterator
		//LOOP - Add entities from XML
		for(entsitr = entsitr.begin(entitiessection);entsitr != entsitr.end();entsitr++)
		{

			//Get the type of entity
			std::string entitytype;
			entsitr->GetValue(&entitytype);
			//IF - Ingame creation
			if(mGameLogicptr)
			{
				//Process differently depending on type
				if(entitytype == "Sprite") //Sprite loading
					_processSpriteEntity(entsitr,filepath);
				else if(entitytype == "BodyAgent") //Physic body agent loading
					_processBodyEntity(entsitr,filepath);
				else if(entitytype == "Joint") //Joint Loading
					_processJointEntity(entsitr,filepath);
				else if(entitytype == "Player") //Player agent Loading
					_processPlayerEntity(entsitr,filepath);
				else if(entitytype == "AIAgent")
					_processAIEntity(entsitr, filepath);
				///HACK
				else if(entitytype == "AntiVirusScanner")  //Antivirus logic
					mGameLogicptr->mAVLogic->BuildFromXML(entsitr.Get());
				///HACK END
			}//ELSE - Assets only
			else if(assetsonly)
			{
				//Only load sprites (level acts as assets containter)
				if(entitytype == "Sprite") //Sprite loading
					_processSpriteEntity(entsitr,filepath);
			}
			/*//ELSE - Editor creation
			else if(mEditorLogicptr)
			{
				//Only load sprites and bodies to render in editor
				if(entitytype == "Sprite") //Sprite loading
					_processSpriteEntity(entsitr,filepath);
				else if(entitytype == "Body") //Physic body loading
					_processBodyEntity(entsitr,filepath);
			}//ELSE - Should never happen!*/
			else
			{
				assert(false);   //Something bad happened!
			}
		}//LOOP END
			
		//Level definition checkings
		//Nothing for the moment
	}
	catch (std::exception& e) //Catch exceptions
	{
		_handleException();  //Delete in-process created data for builder
		throw(GenericException(e.what(),GenericException::FILE_CONFIG_INCORRECT)); //Rethrow exception
	}
	//**************LEVEL LOADED******************************************
}



void LevelBuilder::_processSpriteEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string& filepath)
{
	//Assure Id assigned is correct
	//Id
	std::string Id = theentity->GetAttribute("Id");
	if(
		Id == ""
	    ||
	    mLevelPointer->mEntitiesMap.find(Id) != mLevelPointer->mEntitiesMap.end()
	 )
	 throw GenericException("Failure while reading '" + filepath + "'Id '" + Id + "' not correct (repeated or empty)!",GenericException::FILE_CONFIG_INCORRECT);

	//Create builder object for this type and call construction
	SpriteBuilder thebuilder;
	//It can throw exceptions - manage them!
	try
	{
		//Call creation of object
		thebuilder.ReadSpriteParams(theentity.Get());
		thebuilder.CreateNewSprite();
	}
	catch(GenericException& e)
	{
		//For now, just throw it back with file info
		throw(GenericException("Error loading File '" + filepath +"' :" +e.What_String(),e.GetCode()));
	}		

	//Now add created sprite to container
	mLevelPointer->mEntitiesMap[Id] = thebuilder.GetCreatedSprite().gfxentity;
}

void LevelBuilder::_processBodyEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string& filepath)
{
	
	//Create builder object for this type and call construction
	SolidBodyAgentBuilder thebuilder;
	//It can throw exceptions - manage them!
	try
	{
		//Call creation of object
		thebuilder.ReadParams(theentity.Get());
		thebuilder.CreateAgent(mGameLogicptr->GetPhysicsManager());
	}
	catch(GenericException& e)
	{
		//For now, just throw it back with file info
		throw(GenericException("Error loading File '" + filepath +"' :" +e.What_String(),e.GetCode()));
	}			
	
	/*//Only in in-game an agent gets created
	if(mEditorLogicptr)
	{
		//Append to internal list of editor logic (MORE HACKS)
		mEditorLogicptr->AppendBody(entId); 
	}*/
}

//Utility function to get the vertices data in float format from a string
void LevelBuilder::_processJointEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string& filepath)
{
/*JOINT ELEMENT
 	Element: JOINT Atts: Id (string) Type(string) Body1(string) Body2(string) AllowCollision(number) LimitsOn(number)
			 Element Coordinates: 
				Value : Comma separated list of joint points in local body coordinates (body1x, body1y, body2x,body2y)(4 points) example-> "0,0,0,5"
	*/
	
	//-------Get attributes from entity---------
	//Id
	std::string entId = theentity->GetAttribute("Id");
	if(
		entId == ""
	    ||
	    mLevelPointer->mEntitiesMap.find(entId) != mLevelPointer->mEntitiesMap.end()
	 )
		throw GenericException("Failure while reading '" + filepath + "' Id '"+ entId +"' not correct! (Repeated or empty)",GenericException::FILE_CONFIG_INCORRECT);
	//Attached bodies and type
	std::string body1, body2;
	std::string type;
	//Collision allowance
	bool allowcollision;
	//Type and other attributes
	type = theentity->GetAttribute("Type");
	body1 = theentity->GetAttribute("Body1");
	body2 = theentity->GetAttribute("Body2");
	theentity->GetAttribute("AllowCollision",&allowcollision);
	
	
	ticpp::Element* coordselem = theentity->FirstChildElement("Coordinates");
	std::string coords = coordselem->GetText();
	int parsedcoords = _getNumberofVertices(coords);
	//Checkings
	if ((parsedcoords != 4 && type != "Revolute" && type != "Prismatic")||(parsedcoords != 2 && (type == "Revolute" || type == "Prismatic")) )
		throw GenericException("Failure while reading '" + filepath + "' coordinates for joint '"+entId +"' incorrect",GenericException::FILE_CONFIG_INCORRECT);
	if((allowcollision != 0 && allowcollision != 1))
		throw GenericException("Failure while reading '" + filepath + "' bad joint attributes! - Out of bounds values",GenericException::FILE_CONFIG_INCORRECT);		

	//Values for coordinates declaration
	float xb1,xb2,yb1,yb2;
	//Create the joint
	//IF - Distance Joint
	if(type == "Distance")
	{
		//Get additional data
		_getVerticesData(coords,xb1,yb1,0);
		_getVerticesData(coords,xb2,yb2,1);
		b2Vec2 vecp1(xb1,yb1);
		b2Vec2 vecp2(xb2,yb2);
		
		//Joint definition
		b2DistanceJointDef newjointdef;
		newjointdef.collideConnected = allowcollision;
		
		//Joint creation
		if(!mGameLogicptr->GetPhysicsManager()->CreateDistanceJoint(&newjointdef,entId,body1,body2,vecp1,vecp2))
			throw GenericException("Failure while reading '" + filepath + "' joint '"+entId+"' connects non-existing bodies!" ,GenericException::FILE_CONFIG_INCORRECT);		
	}//ELSE - Revolute Joint
	else if(type == "Revolute")
	{	
		//Get additional data
		_getVerticesData(coords,xb1,yb1,0);
		b2Vec2 vecp(xb1,yb1);
		float maxangle,minangle;
		bool limited;
		theentity->GetAttribute("LimitsOn",&limited);
		theentity->GetAttribute("MaxAngle",&maxangle,limited);  //Optional
		theentity->GetAttribute("MinAngle",&minangle,limited);  //Optional
		//Checkings
		if(maxangle == 0 || minangle == 0)
			throw GenericException("Failure while reading '" + filepath + "' joint '"+entId+"' has invalid angle restrictions" ,GenericException::FILE_CONFIG_INCORRECT);	
		
		//Joint definition
		b2RevoluteJointDef newjointdef;
		newjointdef.collideConnected = allowcollision;
		newjointdef.upperAngle = static_cast<float32>(SingletonMath::Instance()->AngleToRadians(maxangle));
		newjointdef.lowerAngle = static_cast<float32>(SingletonMath::Instance()->AngleToRadians(minangle));
		newjointdef.enableLimit = limited;
		//Joint creation
		if(!mGameLogicptr->GetPhysicsManager()->CreateRevoluteJoint(&newjointdef,entId,body1,body2,vecp))
			throw GenericException("Failure while reading '" + filepath + "' joint '"+entId+"' connects non-existing bodies!" ,GenericException::FILE_CONFIG_INCORRECT);	
	}//ELSE - Prismatic Joint
	else if(type == "Prismatic")
	{
		//Get additional data
		ticpp::Element* axis = theentity->FirstChildElement("Axis");
		std::string axisdata = axis->GetText();
		int numcoords = _getNumberofVertices(axisdata);
		//Checkings
		if (numcoords != 2)
			throw GenericException("Failure while reading '" + filepath + "' axis for joint '"+entId +"' incorrect",GenericException::FILE_CONFIG_INCORRECT);
		float axisx,axisy;
		_getVerticesData(axisdata,axisx,axisy,0);
		b2Vec2 vecaxis(axisx,axisy);
		
		//Joint definition
		b2PrismaticJointDef newjointdef;
		newjointdef.collideConnected = allowcollision;
		_getVerticesData(coords,xb1,yb1,0);
		b2Vec2 vecp(xb1,yb1);

		//Joint creation
		if(!mGameLogicptr->GetPhysicsManager()->CreatePrismaticJoint(&newjointdef,entId,body1,body2,vecp,vecaxis))
			throw GenericException("Failure while reading '" + filepath + "' joint '"+entId+"' connects non-existing bodies!" ,GenericException::FILE_CONFIG_INCORRECT);	
	}
	else //ELSE - NOT GOOD TYPE OF JOINT
	{
		throw GenericException("Failure while reading '" + filepath + "' type attribute for joint incorrect!",GenericException::FILE_CONFIG_INCORRECT);
	}

	//Create entity
	/*IND_Entity2d* newentity = new IND_Entity2d;
	SingletonIndieLib::Instance()->Entity2dManager->Add(layer,newentity);
	mLevelPointer->mEntitiesMap[entId] = newentity;*/
	
	//Finally, create an associated agent to manage this data
	/*SolidBodyPar bodyagentparams;
	bodyagentparams.gfxentity = newentity;
	bodyagentparams.physicbody = body;
	bodyagentparams.position = Vector2(x,y);
	bodyagentparams.rotation = rotation;
	mGameLogicptr->GetAgentsManager()->CreateNewAgent(entId,&bodyagentparams);*/

	SingletonLogMgr::Instance()->AddNewLine("LevelBuilder::_init","Joint '" + entId + "' created",LOGDEBUG);
}

void LevelBuilder::_processPlayerEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string& filepath)
{	
	//Assure Id assigned is correct


	//Create builder object for this type and call construction
	PlayerBuilder thebuilder;

	//It can throw exceptions - manage them!
	try
	{
		//Call creation of object
		thebuilder.ReadPlayerParams(theentity.Get());
		thebuilder.CreatePlayer(mGameLogicptr->GetAgentsManager(),mGameLogicptr->GetPhysicsManager());
	}
	catch(GenericException& e)
	{
		//For now, just throw it back with file info
		throw(GenericException("Error loading File '" + filepath +"' :" +e.What_String(),e.GetCode()));
	}			
}

void LevelBuilder::_processAIEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string& filepath)
{
	//Create builder object for this type and call construction
	AIAgentBuilder thebuilder;

	//It can throw exceptions - manage them!
	try
	{
		//Call creation of object
		thebuilder.ReadAIParams(theentity.Get());
		thebuilder.CreateAIAgent(mGameLogicptr->GetPhysicsManager());
	}
	catch(GenericException& e)
	{
		//For now, just throw it back with file info
		throw(GenericException("Error loading File '" + filepath +"' :" +e.What_String(),e.GetCode()));
	}		
}

void LevelBuilder::_processParallaxLayer(ticpp::Iterator<ticpp::Element> theentity, const std::string& filepath)
{
	/*LAYER:
		Layer: Atts: MaxZoom(number), MinZoom(number)
						  Element : Layer Atts: Number(number) SpeedPercentX, SpeedPercentY, ZoomPercent (numbers) 
												StartPosX(number), StartPosY(number)	
										  Element: SPRITE
	*/

	//Get attributes from parallax layer
	//Id
	int layernumber;
	float speedpercentx,speedpercenty;
	float startx, starty;
	float zoompercent;
	theentity->GetAttribute("Number",&layernumber);
	theentity->GetAttribute("SpeedPercentX",&speedpercentx);
	theentity->GetAttribute("SpeedPercentY",&speedpercenty);
	theentity->GetAttribute("ZoomPercent",&zoompercent);
	theentity->GetAttribute("StartPosX",&startx);
	theentity->GetAttribute("StartPosY",&starty);

	//Check correctness of input parameters (we will suppose startx and starty are reasonable...)
	if(speedpercentx < 0.0f || speedpercenty < 0.0f || layernumber < 0)
		throw GenericException("Failure while reading '" + filepath + "' An attribute in a parallax layer is incorrect",GenericException::FILE_CONFIG_INCORRECT);
	
	//See if this layer number was defined before in this level
	GameLevel::ParallaxListIterator it;
	//LOOP - Check there is no repetition of layer
	for(it = mLevelPointer->mParallaxList.begin(); it != mLevelPointer->mParallaxList.end(); it++)
	{
		if(	(*it).thecamera->GetLayer() == layernumber )
			throw GenericException("Failure while reading '" + filepath + "' Layer numbers repeated in parallax definition!!",GenericException::FILE_CONFIG_INCORRECT);
	}//LOOP END
	
	//-----Create camera----
	std::stringstream cameraname;
	cameraname<<"Layer"<<layernumber;
	Camera2DPointer thecamera = SingletonIndieLib::Instance()->RegisterCamera(cameraname.str(),1.0,Vector2(startx,starty),layernumber);
	thecamera->SetLimitingMoves(false);		//Dont limit moves by AABB (it is a parallax, only visual)
	//Create parallax info
	GameLevel::ParallaxInfo params;
	params.relvelocity = Vector2(speedpercentx,speedpercenty) / 100;
	params.relzoom = zoompercent / 100;
	params.thecamera = thecamera;
	params.startpos =  Vector2(startx,starty);

	//Insert it at the end of list and re-sort it by layer number
	mLevelPointer->mParallaxList.push_front(params);
}

//Utility function to get the vertices data in float format from a string
bool LevelBuilder::_getVerticesData(const std::string& verticesdata,float &xvalue, float &yvalue, int vertex)
{
	int number = 0;
	unsigned int startposition = 0;
	unsigned int nextposition = 0;
	unsigned int position = 0;

	//IF - First vertex
	if(vertex == 0)
	{
		startposition = 0;
		nextposition = verticesdata.find(",");
		//Check found
		if(nextposition == std::string::npos)
			return false;
		else
			nextposition++;
	}//ELSE - Not first vertex
	else
	{
		//LOOP - SEARCH FOR SEPARATOR CHARACTER
		while(position < verticesdata.size())
		{
			//Find an ocurrence of separator character in whole string remaining
			position = verticesdata.find(",",position+1,1);
			//Check found
			if(position != std::string::npos)
				number ++; //Increment num of vertices found
 			else
				return false;

			//IF - Found the vertex asked for (x coord)
			if(static_cast<float>(number/2) == static_cast<float>(vertex))
			//if(number%vertex == 0)
			{
				startposition = position+1;
				nextposition = verticesdata.find(",",startposition,1);
				nextposition++;
				break;
			}
		}//LOOP END
	}
	
	//Check vertices where found
	if(startposition == 0 && nextposition == 0)
		return false;

	//DEBUG ASSERT
	assert(startposition < verticesdata.size() && nextposition < verticesdata.size());

	//From positions inside c-strings, get float data of container data
	xvalue = static_cast<float>(atof(verticesdata.substr(startposition,nextposition-1-startposition).c_str()));
	yvalue = static_cast<float>(atof(verticesdata.substr(nextposition).c_str()));

	return true;
}

//Utility function to parse string and find separator characters to see how many numbers are stored in between
int LevelBuilder::_getNumberofVertices(const std::string& verticesdata)
{
	int number = 0;
	unsigned int position = 0;

	//LOOP - SEARCH A STRING FOR SEPARATOR CHARACTER, UPDATE COUNT OF FOUND
	while(position < verticesdata.size() && position != std::string::npos)
	{
		//Find an ocurrence of separator character in whole string remaining
		position = verticesdata.find(",",position+1,1);
		if(position != std::string::npos)
			number ++;
	}//LOOP END

	return(number + 1);
}

void LevelBuilder::_handleException()
{
	//When exception is thrown, it is necessary to unload all resources from IndieLib properly
	SingletonLogMgr::Instance()->AddNewLine("LevelBuilder","Exception when loading level/ Unloading all entities loaded so far...",LOGEXCEPTION);
	if(mLevelPointer)
		mLevelPointer->_release(); //Call release of level
}
