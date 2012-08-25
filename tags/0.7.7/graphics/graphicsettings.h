/***************************************************************************
                          graphicsettings.h  -  Graphics configuration
                             -------------------
    begin                : vr feb 4 2005
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

#ifndef GRAPHICSETTINGS_H
#define GRAPHICSETTINGS_H

struct SGraphicSettings
{
	bool m_UseBackground;
	bool m_ZBuffer;
	int m_VisibleTiles;
	int m_FogMode;
	int m_MovingObjectLOD;
	enum {off, blend} m_Transparency;
	bool m_TexSmooth;
	bool m_ShadowSmooth;
	float m_ReflectionDist;
	bool m_UpdRef, m_UpdRefAllSides, m_UpdRefAllObjs;
	int m_ReflectionSize, m_ShadowSize;
	bool m_ReflectionDrawMovingObjects;
	bool m_TrackDisplayList;

	bool m_EnableAnimation;
	unsigned m_WaterTesselation;

	bool m_CrashSmoke;
};

#endif
