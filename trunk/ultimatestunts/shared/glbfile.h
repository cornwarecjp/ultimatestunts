/***************************************************************************
                          glbfile.h  -  Interface for the GLB file format
                             -------------------
    begin                : di feb 1 2005
    copyright            : (C) 2005 by CJP
    email                : cornware-cjp@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GLBFILE_H
#define GLBFILE_H

#include <vector>
namespace std {}
using namespace std;

#include "vector.h"
#include "cstring.h"
/**
  *@author CJP
  */

class CGLBFile {
public: 
	CGLBFile();
	~CGLBFile();

	//loading and saving
	bool load(const CString &filename);
	void save(const CString &filename);

	//the data:
	struct SVertex
	{
		CVector pos, nor, tex;
	};
	struct SPrimitive
	{
		vector<SVertex> vertex;
		vector<unsigned int> index;

		int Texture;
		CString Name;
		unsigned int LODs;
		CVector ModulationColor, ReplacementColor;
		float Opacity, Reflectance, Emissivity;
		float StaticFriction, DynamicFriction;
	};
	vector<SPrimitive> m_Primitives;
};

#endif
