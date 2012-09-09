/*****************************************************************************************
/* File: IND_FontManager.cpp
/* Desc: Manager de fuentes
/*****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier L�pez L�pez (javierlopezpro@gmail.com) 

This library is free software; you can redistribute it and/or modify it under the 
terms of the GNU Lesser General Public License as published by the Free Software 
Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with 
this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
Suite 330, Boston, MA 02111-1307 USA 
*/

// ----- Includes -----

#include "Global.h"
#include "Defines.h"
#include "..\Dependencies\TinyXml\tinyxml.h"
#include "IND_FontManager.h"

// ----- Defines -----

#define MAX_TOKEN 1024

// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pImageManager			Pointer to the manager IND_ImageManager correctly initialized
\arg \b pSurfaceManager			Pointer to the manager IND_SurfaceManager correctly initialized

\b Operation:

This function returns 1 (true) if the adminstrator is successfully initialized.
Must be called before using any method.
*/
bool IND_FontManager::Init (IND_ImageManager *pImageManager, IND_SurfaceManager *pSurfaceManager)
{
	End ();
	InitVars ();	

	Debug->Header ("Initializing FontManager", 5);

	// Checking IND_SurfaceManager
	if (pSurfaceManager->IsOk())
	{
		Debug->Header ("SurfaceManager Ok", 1);
		mSurfaceManager = pSurfaceManager;

		Debug->Header ("ImageManager Ok", 1);
		mImageManager = pImageManager;

		mOk = true;

		Debug->Header ("FontManager OK", 6);
	}
	else
	{
		Debug->Header ("SurfaceManager is not correctly initalized", 2);
		mOk = false;
	}

	return mOk;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_FontManager::End()
{
	if (mOk)
	{
		// If the object is loaded, we free it
		Debug->Header ("Finalizing FontManager", 5);
		Debug->Header ("Freeing fonts" , 5);
		FreeVars ();	
		Debug->Header ("Fonts freed", 6);
		Debug->Header ("FontManager finalized", 6);
	
		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//									Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pNewFont				Pointer to a font.
\arg \b pName					Name of the graphic file that contains the font generated by \b MudFont
\arg \b pFile					Name of the configuration file of the font generated by \b MudFont
\arg \b pType					Font type (see ::IND_Type)
\arg \b pQuality				Font quality (see ::IND_Quality)

\b Operation:

This function returns 1 (true) if the font object passed as a parameter 
exists and is added successfully to the manager loading the font directly from
a graphic file and a configuration file, both generated with \b MudFont
(modified version for \b IndieLib that can be found in the \b tools section ).
*/
bool IND_FontManager::Add		(IND_Font		*pNewFont,
								char			*pName,
								char			*pFile,
								IND_Type		pType,
								IND_Quality		pQuality)
{
	// Image loading
	IND_Image *mNewImage = new IND_Image;
	if (!mImageManager->Add (mNewImage, pName))
		return 0;
	
	// IND_Surface creation
	if (!Add (pNewFont, mNewImage, pFile, pType, pQuality))
	{
		mImageManager->Delete (mNewImage);
		return 0;
	}

	// Free the image
	mImageManager->Delete (mNewImage);
	
	return 1;
}


/*!
\b Parameters:

\arg \b pNewFont				Pointer to a new object type 1 font.
\arg \b pImage					Pinter to an object ::IND_Image that contains a previously loaded font from a graphic file generated by \b MudFont (see \b tools section).
\arg \b pFile					Name of the configuration file of the font generated by \b MudFont (see \b tools section)
\arg \b pType					Font type (see ::IND_Type)
\arg \b pQuality				Font quality (see ::IND_Quality)

\b Operation:

This function returns 1 (true) if the font object type 1 passed as a parameter 
exists and is added successfully to the manager loading the font directly from
::IND_Image object and a configuration file, both generated with \b MudFont
(modified version for \b IndieLib can be found in the section \b tools).

The posibility of changing the font from an ::IND_Image object is offered in case 
that you want to change the original font with any modification or filter from 
::IND_ImageManager
*/
bool IND_FontManager::Add		(IND_Font		*pNewFont,
								IND_Image		*pImage,
								char			*pFile,
								IND_Type		pType,
								IND_Quality		pQuality)
{
	Debug->Header ("Parsing and loading font", 5);
	Debug->Header ("File name:", 3);
	Debug->DataChar (pFile, 1);

	if (!mOk)
	{	
		WriteMessage ();
		return 0;
	}
	
	// ----- Width and height of the bitmap font MUST be power of two -----

	IND_Math mMath;

	if (!mMath.IsPowerOfTwo (pImage->GetWidth ()) || 
		!mMath.IsPowerOfTwo (pImage->GetHeight ()))
	{
		Debug->Header ("This operation can not be done", 3);
		Debug->DataChar ("", 1);
		Debug->Header ("The height and width of the font must be power of 2", 2);
		return 0;
	}

	// ----- Bitmap (IND_Surface object) creation -----

	IND_Surface *mNewSurface = new IND_Surface;
	if (!mSurfaceManager->Add (mNewSurface, pImage, pType, pQuality)) 
		return 0;

	// IND_Surface object MUST have one block ONLY
	if (mNewSurface->GetNumBlocks() > 1)
	{
		mSurfaceManager->Delete (mNewSurface);
		return 0;
	}

	pNewFont->SetSurface (mNewSurface);

	// ----- XML font parsing -----

	if (!ParseFont (pNewFont, pFile))
	{
		Debug->Header ("Fatal error, cannot load the font xml file", 2);
		return 0;
	}

	pNewFont->SetFileName (pFile);

	// ----- Puts the object into the manager -----

	AddToList (pNewFont);
	
	// ----- Debug -----

	Debug->Header("Font parsed and loaded", 6);	
	
	return 1;
}


/*!
\b Parameters:

\arg \b pFo                Pointer to font object type 1

\b Operation:  

This function returns 1 (true) if the font object type 1 passed as a parameter exists
and it is deleted from the manager successfully
*/
bool IND_FontManager::Delete (IND_Font	*pFo)
{
	Debug->Header ("Freeing font", 5);

	if (!mOk)
	{	
		WriteMessage ();
		return 0;
	}

	// Search object
	bool mIs = 0;
	list <IND_Font*>::iterator mFontListIter;
	for (mFontListIter  = mListFonts->begin();
		 mFontListIter != mListFonts->end();
		 mFontListIter++)
	{	
		if ((*mFontListIter) == pFo) 
		{
			mIs = 1;
			break;
		}
	}

	// Not found
	if (!mIs)
	{
		WriteMessage ();
		return 0;
	}

	// ----- Free object -----

	Debug->Header ("File name:", 3);
	Debug->DataChar (pFo->GetFileName (), 1);	

	// Quit from list
	DelFromList (pFo);

	// Free bitmap IND_Surface
	mSurfaceManager->Delete (pFo->GetSurface());

	// Free letter array
	DisposeArray (pFo->mFont.mLetters);

	Debug->Header ("Ok", 6);

	return 1;
}

// --------------------------------------------------------------------------------
//									Private methods
// --------------------------------------------------------------------------------

/*
==================
Parses and XML font file
Uses Tinyxml
==================
*/
bool IND_FontManager::ParseFont (IND_Font *pNewFont, char *pFontName)
{
	TiXmlDocument	*mXmlDoc = new TiXmlDocument (pFontName);

	// Fatal error, cannot load
	if (!mXmlDoc->LoadFile()) return 0;

	// Document root
	TiXmlElement *mXFont = 0;
	mXFont = mXmlDoc->FirstChildElement("font");

	if (!mXFont)
	{
		Debug->Header ("Invalid name for document root, should be <font>", 2);
		mXmlDoc->Clear();
		delete mXmlDoc;
		return 0;
	}

	if (mXFont->Attribute("num_characters"))
	{
		pNewFont->mFont.mNumChars = atoi (mXFont->Attribute("num_characters"));
		pNewFont->SetLetters (new IND_Font::LETTER [pNewFont->GetNumChars()]);
	}
	else
	{
		Debug->Header ("The font doesn't have a \"num_characters\" attribute", 2);
		mXmlDoc->Clear();
		delete mXmlDoc;
		return 0;
	}

	TiXmlElement *mXChar= 0;
	mXChar = mXFont->FirstChildElement("char");

	if (!mXChar)
	{
		Debug->Header ("There are no chars to parse", 2);
		mXmlDoc->Clear();
		delete mXmlDoc;
		return 0;
	}

	// Parse all the chars
	int mCont = 0;
	while (mXChar)
	{
		// Id
		if (mXChar->Attribute("id"))
		{
			pNewFont->GetLetters() [mCont].mLetter = atoi (mXChar->Attribute("id"));
		}
		else
		{
			Debug->Header ("The char doesn't have a \"id\" attribute", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}

		// x
		if (mXChar->Attribute("x"))
		{
			pNewFont->GetLetters() [mCont].mOffsetX = atoi (mXChar->Attribute("x"));
		}
		else
		{
			Debug->Header ("The char doesn't have a \"x\" attribute", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}

		// y
		if (mXChar->Attribute("y"))
		{
			pNewFont->GetLetters() [mCont].mOffsetY = atoi (mXChar->Attribute("y"));
		}
		else
		{
			Debug->Header ("The char doesn't have a \"y\" attribute", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}

		// width
		if (mXChar->Attribute("width"))
		{
			pNewFont->GetLetters() [mCont].mWidthChar = atoi (mXChar->Attribute("width"));
		}
		else
		{
			Debug->Header ("The char doesn't have a \"width\" attribute", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}

		// height
		if (mXChar->Attribute("height"))
		{
			pNewFont->GetLetters() [mCont].mHeightChar = atoi (mXChar->Attribute("height"));
		}
		else
		{
			Debug->Header ("The char doesn't have a \"height\" attribute", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}

		// Move to the next char declaration
		mXChar = mXChar->NextSiblingElement("char");

		mCont++;
	}

  	mXmlDoc->Clear();
	delete mXmlDoc;

	return 1;
}


/*
==================
Inserts the font into a list
==================
*/
void IND_FontManager::AddToList	(IND_Font *pNewFont)
{
	mListFonts->push_back (pNewFont);
}


/*
==================
Deletes object from the manager
==================
*/
void IND_FontManager::DelFromList (IND_Font *pFo)
{
	mListFonts->remove (pFo);
	pFo = NULL;
}



/*
==================
Initialization error message
==================
*/
void IND_FontManager::WriteMessage ()
{
	Debug->Header ("This operation can not be done", 3);
	Debug->DataChar ("", 1);
	Debug->Header ("Invalid Id or FontManager not correctly initialized", 2);
}


/*
==================
Init manager vars
==================
*/
void IND_FontManager::InitVars ()
{
	mListFonts = new list <IND_Font*>;
}


/*
==================
Free manager memory
==================
*/
void IND_FontManager::FreeVars ()
{
	// Deletes all the manager entities
	list <IND_Font*>::iterator mFontListIter;
	for (mFontListIter  = mListFonts->begin();
		 mFontListIter != mListFonts->end();
		 mFontListIter++)
	{	
		Debug->Header	("Freeing font:", 3);
		Debug->DataChar ((*mFontListIter)->GetFileName(), 1);

		// Free bitmap IND_Surface
		mSurfaceManager->Delete ((*mFontListIter)->GetSurface ());	

		// Free letter array
		DisposeArray ((*mFontListIter)->mFont.mLetters);
	}

	// Clear list
	mListFonts->clear ();

	// Free list
	Dispose (mListFonts);
}
