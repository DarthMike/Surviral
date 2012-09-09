/*
	Filename: PhysicsBodyBuilder.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric physics body
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "PhysicsBodyBuilder.h"
#include "PhysicsManager.h"
#include "Math.h"

//Definition of static elements
int PhysicsBodyBuilder::mBodiesBuilt = 0;

//Read a part of xml info
void PhysicsBodyBuilder::ReadBodyParams(const ticpp::Element* xmlelement)   
{
/*BODY ELEMENT
 	Element: BODY Atts: Id (string) x(number) y(number) h(number) w(number) Rotation(number) Transp(number) FlipX(number) FlipY(number) 
	                    Layer(number) LinDamping(number) AngDamping(number) IsStatic(number) [FixedRotation(number)]
			  [Element: Sprite]
			 [Elements(more than one): PolygonShape Atts: Order(number) Density(number) Friction(number) Restitution(number) [CollGroupIndex(number) CollMaskBits(number) CollCategoryBits(number)]]
				Value : Comma separated list of points (2xOrder elements) example-> order 4: "0,0,0,5,5,5,5,0"
			 [Elements(more than one): CircleShape Atts: Radius(number) Density(number) Friction(number) Restitution(number) [CollGroupIndex(number) CollMaskBits(number) CollCategoryBits(number)]]
				Value : Comma separated list of CENTER POINT  example-> order 4: "0,0"
	*/
	
	//-------Get attributes from entity---------
	//Id
	mParams.id = xmlelement->GetAttribute("Id");	  //NOTE: Not checking, as it is done in physics Manager
	//Position / size
	float x,y,rotation;
	float lindamping,angdamping;
	bool isstatic;
	bool fixedrotation (false);  //Optional
	xmlelement->GetAttribute("x",&x);
	xmlelement->GetAttribute("y",&y);
	xmlelement->GetAttribute("Rotation",&rotation);
	xmlelement->GetAttribute("LinDamping",&lindamping);
	xmlelement->GetAttribute("AngDamping",&angdamping);
	xmlelement->GetAttribute("IsStatic",&isstatic);
	xmlelement->GetAttribute("FixedRotation",&fixedrotation,false);
	
	if(lindamping < 0 || lindamping > 1 || angdamping < 0 || angdamping > 1 || (isstatic != 0 && isstatic != 1))
		throw GenericException("Bad attributes detected! entity '" + mParams.id + "': Out of bounds values",GenericException::FILE_CONFIG_INCORRECT);

	//A BODY entity is quite complicated; it can internally have many sprites, and
	//it can have many shapes associated, and additionally, it MUST HAVE physical properties.
	mParams.bodydef.allowSleep = true;
	mParams.bodydef.position = b2Vec2(x,y);  //Position data
	mParams.bodydef.angle = static_cast<float32>(SingletonMath::Instance()->AngleToRadians(rotation));
	mParams.bodydef.isBullet = false;			
	mParams.bodydef.linearDamping = static_cast<float32>(lindamping);
	mParams.bodydef.angularDamping = static_cast<float32>(angdamping);
	mParams.bodydef.fixedRotation = fixedrotation;
	mParams.isstatic = isstatic;

	//------Get elements associated to entity-------
	ticpp::Iterator <ticpp::Element> entelement;
	//LOOP - Get elements of entity
	for(entelement = entelement.begin(xmlelement);entelement != entelement.end();entelement++)
	{
		//IF - Entity type is a polygon
		if(entelement->Value() == "PolygonShape")
		{
			//Density, restitution, and Friction
			float32 density, restitution, friction;
			entelement->GetAttribute("Density",&density);
			entelement->GetAttribute("Restitution",&restitution);
			entelement->GetAttribute("Friction",&friction);
		
			//Sensor?
			bool sensor(false);
			entelement->GetAttribute("Sensor",&sensor,false);  //Optional param
			//Filters for collision (optional)
			int16 groupindex (0);
			uint16 mask (0xFFFF); 
			uint16 category (0x0001);
			entelement->GetAttribute("CollGroupIndex",&groupindex,false);  //optional
			entelement->GetAttribute("CollMaskBits",&mask,false); //optional
			entelement->GetAttribute("CollCategoryBits",&category,false); //optional
			//Order
			int32 order;
			entelement->GetAttribute("Order",&order);
			//Vertices data
			std::string verticesdata = entelement->GetText();
			int parsedvertices = _getNumberofVertices(verticesdata);
			//Checkings
			if(order > b2_maxPolygonVertices || order < 3  || order != parsedvertices/2 || density < 0 || restitution < 0 || friction < 0)
				throw GenericException("Body '" + mParams.id + "' Bad attributes for PolygonShape element",GenericException::FILE_CONFIG_INCORRECT);
			
			b2PolygonDef newpolygondef;
			newpolygondef.density = density;
			newpolygondef.restitution = restitution;
			newpolygondef.friction = friction;
			newpolygondef.vertexCount = order;
			newpolygondef.isSensor = sensor;
			newpolygondef.filter.groupIndex = groupindex;
			newpolygondef.filter.maskBits = mask;
			newpolygondef.filter.categoryBits = category;

			//LOOP - Set vertices data of polygon
			for(int i = 0; i<order; i++)
			{
				float x,y;
				if(!_getVerticesData(verticesdata,x,y,i))
					throw GenericException("Body '" + mParams.id + "' Data invalid for polygon shape definition",GenericException::FILE_CONFIG_INCORRECT);
				
				newpolygondef.vertices[i].Set(x,y);
			}//LOOP

			//Store polygon shape definition
			mParams.definedpolygonshapes.push_back(newpolygondef);
		}//ELSE - FOUND CIRCULAR SHAPE
		else if(entelement->Value() == "CircleShape")
		{
			//Density, restitution, and Friction, and Radius
			float32 density, restitution, friction, radius;
			entelement->GetAttribute("Density",&density);
			entelement->GetAttribute("Restitution",&restitution);
			entelement->GetAttribute("Friction",&friction);
			entelement->GetAttribute("Radius",&radius);
			//Sensor?
			bool sensor(false);
			entelement->GetAttribute("Sensor",&sensor,false);  //Optional param

			//Vertices data - IN THIS CASE IS THE CENTER OF CIRCLE IN LOCAL COORDS
			std::string verticesdata = entelement->GetText();
			int parsedvertices = _getNumberofVertices(verticesdata);
			//Checkings
			if( parsedvertices != 2 || density < 0 || restitution < 0 || friction < 0)
				throw GenericException("Body '" + mParams.id + "' Bad attributes for CircleShape element",GenericException::FILE_CONFIG_INCORRECT);

			b2CircleDef newcircledef;
			newcircledef.density = density;
			newcircledef.restitution = restitution;
			newcircledef.friction = friction;
			newcircledef.radius = radius;
			newcircledef.isSensor = sensor;
			
			float32 x,y;
			_getVerticesData(verticesdata,x,y,0);
			b2Vec2 pos(x,y);
			newcircledef.localPosition = pos;
			
			//Store circle shape definition
			mParams.definedcircleshapes.push_back(newcircledef);		
		}//IF		
	}//LOOP END
	
	mParamsLoaded = true;
}

//Create a new body - If refactoring, a new name is chosen for everything (to create a new element from 
//a template in this builder with different procedurally-generated name)
void PhysicsBodyBuilder::CreateNewBody(PhysicsManagerPointer physicsmgrptr, bool refactor)
{

	assert (mParamsLoaded);

	//Create the body with the loaded definition
	std::stringstream appendedid;
	if(refactor)
		appendedid<<"_DYNAMIC_"<<mBodiesBuilt;

	mCreatedBody = physicsmgrptr->CreateBody(&mParams.bodydef,mParams.id + appendedid.str());

	if(mCreatedBody == NULL)
		throw GenericException("' Id '"+ mParams.id +"' for body not correct! (Repeated or empty)",GenericException::FILE_CONFIG_INCORRECT);

	//Create shapes to this body
	//Polygon shapes
	PolygonShapeDefVec::iterator polshapeitr;

	//LOOP - Create all polygon shapes
	for(polshapeitr = mParams.definedpolygonshapes.begin();polshapeitr != mParams.definedpolygonshapes.end(); ++polshapeitr)
	{
		//Create a new shape to body
		physicsmgrptr->CreatePolygonShape(&(*polshapeitr),mParams.id + appendedid.str());
	}
	
	CircleShapeDefVec::iterator circshapeitr;
	//LOOP - Create all circle shapes
	for(circshapeitr = mParams.definedcircleshapes.begin();circshapeitr != mParams.definedcircleshapes.end(); ++circshapeitr)
	{
		//Create a new shape to body
		physicsmgrptr->CreateCircleShape(&(*circshapeitr),mParams.id + appendedid.str());
	}

	//When entity was created, compute mass from shapes if it is a movable body
	if(!mParams.isstatic )
		mCreatedBody->SetMassFromShapes();

	++mBodiesBuilt;

}

//Utility function to get the vertices data in float format from a string
bool PhysicsBodyBuilder::_getVerticesData(const std::string& verticesdata,float &xvalue, float &yvalue, int vertex)
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
int PhysicsBodyBuilder::_getNumberofVertices(const std::string& verticesdata)
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