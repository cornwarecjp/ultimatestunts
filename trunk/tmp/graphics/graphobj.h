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
#include "graphicsettings.h"
#include "reflection.h"

class CGraphObj : public CDataObject
{
	public:
		CGraphObj(CDataManager *manager, eDataType type);
		virtual ~CGraphObj();

		virtual bool load(const CString &filename, const CParamList &list);
		virtual void unload();

		void draw(const SGraphicSettings *settings, CReflection *reflection,
			unsigned int lod, float t, bool useMaterials=true);

	protected:
		//Rendering settings
		const SGraphicSettings *m_CurrentSettings;
		CReflection *m_CurrentReflection;
		unsigned int m_CurrentLOD;
		float m_CurrentTime;
		bool m_UseMaterials;

		//Primitive data
		struct SPrimitive
		{
			unsigned int LODs;
			
			unsigned int texture[4];
			CVector color[4];
			float opacity, reflectance, emissivity;
			
			//Original position+normal, for animation purposes
			vector<CVector> originalPos, originalNor;
			
			void *vertex;
			unsigned int numVertices;

			void *index;
			unsigned int numIndices;

			//Animation
			struct
			{
				bool rotationEnabled; CVector rotationOrigin, rotationVelocity;
			} animation;
		};
		vector<SPrimitive> m_Primitives;

		void unloadPrimitive(SPrimitive &pr);

		//Draw one primitive:
		void drawPrimitive(const SPrimitive &pr);

		//returns false for fully transparent materials
		bool setMaterial(const SPrimitive &pr, bool forReflection=false);

		//Animates the primitives
		virtual void animate(float t);

		void animateWater(SPrimitive &pr, float t);
		float m_WaterLevel;

		void animateRotation(SPrimitive &pr, float t);
};

#endif

