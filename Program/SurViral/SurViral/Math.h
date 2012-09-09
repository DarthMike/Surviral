/*
	Filename: Math.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate useful math operations
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _MATH
#define _MATH

//Library dependencies
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <algorithm>
//Class dependencies
#include "Singleton_Template.h"
#include "Vector2.h"
#include "Vector3.h"

using namespace std;

class Math : public MeyersSingleton<Math>
{
	//Definitions
public:
	static const double MaxDegrees;
	static const double MinDegrees;
	static const double Pi;
	static const double Two_Pi;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	Math()
	{
		//Initialize random number generator
		srand ((unsigned int) time(NULL) );
	}
	~Math()
	{}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Returns a random number between boundaries
	inline int NewRandom(int low, int high)
	{
		//Modulo operation to transform 0-RAND_MAX to
		//0 - high, then sum of minimum range
		if(high>low)
		{
			return((rand() % (high - low)) + low);
		}
		else
		{	//If not correct parameters a 0 is returned
			return(0);
		}	
	}
	
	//Returns a random number between -1 and 1 (templated)
	template <typename type>
	inline type ClampedRandom()				
	{
		//Perform static cast to type - compile error if not possible
		return (static_cast<type>(NewRandom(-1,1)));
	}

	//Values clamping to max
	inline static Vector2 ClampVector2(Vector2& vector,double maxvalue,double minvalue)	
	{
		//Clamp coords
		vector.x = ClampNumber(vector.x,maxvalue,minvalue);
		vector.y = ClampNumber(vector.y,maxvalue,minvalue);

		return Vector2(vector.x,vector.y);   //Returns a new vector
	}

	//Find a perpendicular vector given a value > or < 0
	inline static Vector2 FindPerpendicularVector2(const Vector2& vector, float value)
	{
		//If value is > 0 , it returns one direction perpendicular vector, 
		//otherwise, returns the other one

		Vector2 returnvector;

		if(value <= 0)
		{
			double temp = vector.x;
			returnvector.x = vector.y;
			returnvector.y = -temp;
		}
		else
		{
			double temp = vector.x;
			returnvector.x = -vector.y;
			returnvector.y = temp;
		}

		return (returnvector);
	}

	//Angle between vectors
	template <typename type>
	inline static void SignedAngleBetweenVecs(const Vector2& v1, const Vector2& v2, type& resultangle)
	{
		double angle (atan2(v2.y,v2.x) - atan2(v1.y,v1.x));  //Angle with signed orientation angle2 - angle1
		
		if(abs(angle) < 0.0001f)   //Floating point roundups for zero angles
			angle = 0.0f;

		resultangle =  static_cast<type>(angle);
	}

	//Find a line to line intersection
	inline static bool LineToLineIntersection(const Vector2& line1p1, const Vector2& line1p2, const Vector2& line2p1, const Vector2& line2p2, Vector2& outintersection) 
	{
		//From the net - line to line intersection; NOTE, NOT OPTIMIZED
		// Store the values for fast access and easy
		// equations-to-code conversion
		double x1 = line1p1.x, x2 = line1p2.x, x3 = line2p1.x, x4 = line2p2.x;
		double y1 = line1p1.y, y2 = line1p2.y, y3 = line2p1.y, y4 = line2p2.y;
	 
		//Determinant calculation
		double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		// If determinant is zero, there is no intersection (parallel)
		if (d == 0) return false;
	 
		//Get the x and y
		double pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
		double x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
		double y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
	 
		//Check if the x and y coordinates are within both lines
		if ( x < min(x1, x2) 
			|| x > max(x1, x2) 
			 || x < min(x3, x4) 
			 || x > max(x3, x4) ) 
			 return false;
	    
		if ( y < min(y1, y2) 
			 || y > max(y1, y2) 
			 || y < min(y3, y4) 
			 || y > max(y3, y4) ) 
			 return false;
	 
		//Store the point of intersection
		outintersection.x = x;
		outintersection.y = y;

		return true;
	}

	//Values clamping between max and min (templated)
	template <typename type>
	inline static type ClampNumber(type num, type max, type min)
	{
		assert(max>min);
		if(num>max)
		{
			return(max);
		}
		else if( num < min)
		{
			return (min);
		}
		else
		{
			return (num);
		}
	}

	//Radians to angle (templated)
	template <typename type>
	inline static type RadiansToAngle(type rads, bool invert = false)	
	{
		type angle; //Angle to return

		//Inversion of angle results
		if(invert)
		{
			angle = (-1) * rads * static_cast<type>(MaxDegrees / Two_Pi);
		}
		else
		{
			angle = rads * static_cast<type>(MaxDegrees / Two_Pi);
		}
		return angle;
	}

	//Angle to radians (templated)
	template <typename type>
	inline static type AngleToRadians(type angle, bool invert = false)
	{
		type radians; //Radians to return

		//Inversion of radians result
		if(invert)
		{
			radians = (-1) * angle * static_cast<type>(Two_Pi / MaxDegrees);
		}
		else
		{
			radians = angle * static_cast<type>(Two_Pi / MaxDegrees);
		}

		return radians;
	}
};

//Definitions - SINGLETON MATH OBJECT
typedef Math SingletonMath;

#endif