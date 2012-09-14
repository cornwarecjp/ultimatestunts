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
#include "binbuffer.h"

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

		CString Name;

		//Material data
		struct
		{
			int Texture;
			unsigned int LODs;
			CVector ModulationColor, ReplacementColor;
			float Opacity, Reflectance, Emissivity;
		} material;

		//Animation data
		struct
		{
			unsigned int AnimationFlags;
			CVector rotationOrigin, rotationVelocity;
			float texturePeriod; vector<int> textures;
		} animation;
	};
	vector<SPrimitive> m_Primitives;

protected:
	bool processObject(unsigned int type, const CString &name, CBinBuffer &data);

	bool processGeometry_05(const CString &name, CBinBuffer &data);
	bool processGeometry_07(const CString &name, CBinBuffer &data);

	bool processVertices(SPrimitive &pr, CBinBuffer &data, unsigned int numVertices);
	bool processIndices (SPrimitive &pr, CBinBuffer &data, unsigned int numIndices );
};

void tesselateSquare(CGLBFile::SPrimitive &pr, unsigned int tess);

#endif
