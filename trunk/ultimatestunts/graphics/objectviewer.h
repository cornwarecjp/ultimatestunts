/***************************************************************************
                          objectviewer.h  -  Viewing CGraphObj objects
                             -------------------
    begin                : wo sep 06 2006
    copyright            : (C) 2006 by CJP
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

#ifndef OBJECTVIEWER_H
#define OBJECTVIEWER_H

#include <vector>
namespace std {}
using namespace std;

#include "renderer.h"
#include "datamanager.h"
#include "staticreflection.h"

/**
  *@author CJP
  */

class CObjectViewer : public CRenderer {
public: 
	CObjectViewer(const CWinSystem *winsys, CDataManager *world);
	virtual ~CObjectViewer();

	virtual void update();

	virtual void reloadData(); //updates pointers to objects in world

	struct SObject
	{
		CString filename;
		CParamList parameters;
		CDataObject::eDataType type;
		int objectID; //updated by reloadData
		CVector position;
		CMatrix orientation;
		bool reflectInGround;
	};
	vector<SObject> m_Objects;
	void addObject(
		const CString &filename, CParamList list, CVector pos,
		CMatrix ori, bool reflect, CDataObject::eDataType type, int replace=-1);

	struct STexture
	{
		CString filename;
		int textureID; //updated by reloadData
	};
	vector<STexture> m_Textures;
	void addTexture(const CString &filename);

	void setReflection(const CString &filename);
protected:
	virtual void updateScreenSize();

	CStaticReflection *m_Reflection;

	CDataManager *m_World;

private:
	void viewObjects(bool isReflection);
};

#endif
