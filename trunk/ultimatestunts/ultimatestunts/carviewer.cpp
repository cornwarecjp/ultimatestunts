/***************************************************************************
                          carviewer.cpp  -  Viewing cars
                             -------------------
    begin                : ma okt 30 2006
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
#include <cstdio>
#include <cmath>

#ifndef M_PI
#define M_PI 3.1415926536
#endif

#include "carviewer.h"

#include "datafile.h"
#include "lconfig.h"
#include "timer.h"

CCarViewer::CCarViewer(const CWinSystem *winsys, CGraphicWorld *world) : CObjectViewer(winsys, world)
{
	CObjectViewer::m_Camera = &m_Camera;

	CVector pos(5.0,5.0,-10.0);
	CMatrix ori;
	ori.targetZ(pos, true);

	m_Camera.setPosition(pos);
	m_Camera.setOrientation(ori);

	setReflection("environment/spheremap.rgb");
}

CCarViewer::~CCarViewer()
{
}

void CCarViewer::update()
{
	CTimer t;
	float time = t.getTime();

	CVector pos(10.0*sin(0.5*time),5.0,10.0*cos(0.5*time));
	CMatrix ori;
	ori.targetZ(pos, true);

	m_Camera.setPosition(pos);
	m_Camera.setOrientation(ori);

	CObjectViewer::update();
}

void CCarViewer::loadCar(const CString &carfile, CVector color)
{
	CDataFile dfile(carfile);
	CLConfig carconf(dfile.useExtern());

	m_Textures.clear();

	addTexture(carconf.getValue("skin", "texture"));
	int texID = m_Textures[0].textureID;


	CParamList params;
	SParameter p;
	p.name = "color";
	p.value = color;
	params.push_back(p);
	p.name = "subset";
	p.value = texID;
	params.push_back(p);

	CMatrix mleft, mright;
	mright.rotY(M_PI);
	CVector pos;
	CString geometry;

	//Body:
	addObject(carconf.getValue("body", "geometry"), params, CVector(), CMatrix(), 0);

	//Right front wheel
	pos = carconf.getValue("frontwheels", "position").toVector();
	geometry = carconf.getValue("frontwheels", "geometry");
	addObject(geometry, params, pos, mright, 1);

	//Left front wheel
	pos.x = -pos.x;
	addObject(geometry, params, pos, mleft, 2);

	//Right rear wheel
	pos = carconf.getValue("rearwheels", "position").toVector();
	geometry = carconf.getValue("rearwheels", "geometry");
	addObject(geometry, params, pos, mright, 3);

	//Left rear wheel
	pos.x = -pos.x;
	addObject(geometry, params, pos, mleft, 4);
}

void CCarViewer::reloadData()
{
	int texID = m_World->loadObject(
			m_Textures[0].filename, CParamList(), CDataObject::eMaterial);

	for(unsigned int i=0; i < m_Objects.size(); i++)
	{
		for(unsigned int j=0; j < m_Objects[i].parameters.size(); j++)
			if(m_Objects[i].parameters[j].name == "subset")
				m_Objects[i].parameters[j].value = texID;
	}

	CObjectViewer::reloadData();
}
