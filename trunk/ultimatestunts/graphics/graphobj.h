/***************************************************************************
                          graphobj.h  -  Graphics geometry object
                             -------------------
    begin                : Tue Apr 23 2002
    copyright            : (C) 2002 by CJP
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
#ifndef GRAPHOBJ_H
#define GRAPHOBJ_H

#include <vector>
namespace std {}
using namespace std;

#include "cstring.h"
#include "vector.h"
#include "dataobject.h"

class CGraphObj : public CDataObject
{
	public:
		CGraphObj(CDataManager *manager, eDataType type);
		virtual ~CGraphObj();

		virtual bool load(const CString &filename, const CParamList &list);
		virtual void unload();

		void draw(int lod) const;

	protected:

		struct SPrimitive
		{
			unsigned int LODs;
			
			unsigned int texture[4];
			CVector color[4];
			float opacity, reflectance, emissivity;
			
			void *vertex;
			unsigned int numVertices;

			void *index;
			unsigned int numIndices;
		};
		vector<SPrimitive> m_Primitives;

		void unloadPrimitive(SPrimitive &pr);

		//Draw one primitive:
		void drawArray(void *vertex, void *index, unsigned int numV, unsigned int numI) const;
};

#endif

