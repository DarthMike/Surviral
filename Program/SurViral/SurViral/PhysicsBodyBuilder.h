/*
	Filename: PhysicsBodyBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric physics body
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _PHYSICSBODYBUILDER
#define _PHYSICSBODYBUILDER

//Library dependencies	
#include "boost\shared_ptr.hpp"  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...

#include <string>
#include <vector>
//Class dependencies
#include "XMLParser.h"
#include "LogManager.h"
#include "GenericException.h"
#include "Shared_Resources.h"
#include "PhysicsManager.h"

class PhysicsBodyBuilder
{
private:

	//A vector containing polygon shapes definitions
	typedef std::vector<b2PolygonDef> PolygonShapeDefVec;

	//A vector containing circular shapes definitions
	typedef std::vector<b2CircleDef> CircleShapeDefVec;

	//Creation parameters for physics body
	typedef struct PhysicsBodyParams{
		PhysicsBodyParams():
		isstatic(false),
		id("")
		{}
		std::string id;
		//Body parameters
		b2BodyDef bodydef;   //BOX2D definition
		bool isstatic;
		//Shape parameters
		//Defined polygonal shapes
		PolygonShapeDefVec definedpolygonshapes;
		//Defined circular shapes
		CircleShapeDefVec definedcircleshapes;
	} PhysicsBodyParams;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	PhysicsBodyBuilder():
	mParamsLoaded(false),
	mCreatedBody(NULL)
	{
	}
	~PhysicsBodyBuilder()
	{
	}
	//----- GET/SET FUNCTIONS -----
	const PhysicsBodyParams& GetBodyParams() { return mParams; }  //Returns A CONST REFERENCE of the created body params
	b2Body* GetCreatedBody() { assert (mParamsLoaded); return mCreatedBody; }
	//----- OTHER FUNCTIONS -----
	void ReadBodyParams(const ticpp::Element* xmlelement);   //Read a part of xml info
	void CreateNewBody(PhysicsManagerPointer physicsmgrptr, bool refactor = false);   //Create a new body
protected:
	//----- INTERNAL VARIABLES -----
	PhysicsBodyParams mParams;
	b2Body* mCreatedBody;
	bool mParamsLoaded;

	static int mBodiesBuilt;
	//----- INTERNAL FUNCTIONS -----
	bool _getVerticesData(const std::string& verticesdata,float &xvalue, float &yvalue, int vertex);  
	int _getNumberofVertices(const std::string& verticesdata);
};

#endif