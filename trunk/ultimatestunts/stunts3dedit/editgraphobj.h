/***************************************************************************
                          editgraphobj.h  -  An editable graphics object
                             -------------------
    begin                : wo apr 16 2003
    copyright            : (C) 2003 by CJP
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

#ifndef EDITGRAPHOBJ_H
#define EDITGRAPHOBJ_H

#include <GL/gl.h>

#include <vector>
namespace std {}
using namespace std;

//#include "graphobj.h"
#include "lodtexture.h"

/**
  *@author CJP
  */

class CVertex {
public:
	CVector pos, nor, tex; //position, normal, texcoord

	//old properties (for backward compatibility)
	CVector col;
	float opacity, reflectance;
};

class CPrimitive {
public:
	enum eType
	{
		Triangles = GL_TRIANGLES,
		Quads = GL_QUADS,
		TriangleStrip = GL_TRIANGLE_STRIP,
		QuadStrip = GL_QUAD_STRIP,
		Polygon =GL_POLYGON,
		VertexArray
	} m_Type;
	
	int m_Texture;
	CString m_Name, m_LODs;
	vector<CVertex> m_Vertex;

	//For vertex array objects:
	vector<unsigned int> m_Index;
	CVector m_ModulationColor, m_ReplacementColor;
	float m_Opacity, m_Reflectance, m_Emissivity;
	float m_StaticFriction, m_DynamicFriction;
};

class CEditGraphObj  { //no longer derived from CGraphObj
public: 
	CEditGraphObj();
	virtual ~CEditGraphObj();

	virtual bool loadGLTFile(CString filename);
	virtual bool loadGLBFile(CString filename);
	bool loadRAWFile(CString filename);
	bool load3DSFile(CString filename);
	bool loadLWOFile(CString filename);
	bool loadOBJFile(CString filename);

	void merge(const CEditGraphObj &obj, const CString &lods);

	//update the display lists
	void render(const CString &visibleLODs); //updates openGL data when primitives are changed

	//draw the display lists
	void draw() const;
	void drawRef() const;

	void convertToVertexArrays();

	void clear();
	void saveGLTFile(const CString &filename);
	void saveGLBFile(const CString &filename);

	vector<CPrimitive> m_Primitives;

	CLODTexture **m_MatArray;
protected:
	unsigned int m_ObjList, m_ObjListRef;

	bool isRendered;

	unsigned int findOrAdd(CPrimitive &pr, const CVertex &v, unsigned int &writeIndex);
	bool isEqual(const CVertex &v1, const CVertex &v2);

	float m_OpacityState;
	CVector m_ColorState;
	void setMaterialColor();
};

#endif
