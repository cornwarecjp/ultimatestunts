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

#include <vector>
namespace std {}
using namespace std;

#include "graphobj.h"

/**
  *@author CJP
  */

class CVertex {
public:
	CVector pos, nor, col, tex; //position, normal, color, texcoord
	float opacity, reflectance;
};

class CPrimitive {
public:
	int m_Type, m_Texture;
	CString m_Name, m_LODs;
	vector<CVertex> m_Vertex;
};

class CEditGraphObj : public CGraphObj  {
public: 
	CEditGraphObj();
	virtual ~CEditGraphObj();
	virtual bool loadFromFile(CString filename, CLODTexture **matarray);
	bool import_raw(CString filename, CLODTexture **matarray);
	bool import_3ds(CString filename, CLODTexture **matarray);

	void merge(const CEditGraphObj &obj, const CString &lods);

	void render(const CString &visibleLODs); //updates openGL data when primitives are changed

	void clear();
	void saveToFile(CString filename);

	vector<CPrimitive> m_Primitives;

protected:
	bool isRendered;
	CLODTexture **m_MatArray;
};

#endif
