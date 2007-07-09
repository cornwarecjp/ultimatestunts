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
#include "pi.h"

#include "carviewer.h"

#include "datafile.h"
#include "bound.h"
#include "lconfig.h"
#include "timer.h"

CCarViewer::CCarViewer(const CWinSystem *winsys, CDataManager *world) : CObjectViewer(winsys, world)
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

	CString frontGeometry, rearGeometry;
	CVector frontPos, rearPos;
	frontGeometry = carconf.getValue("frontwheels", "geometry");
	frontPos = carconf.getValue("frontwheels", "position").toVector();
	rearGeometry = carconf.getValue("rearwheels", "geometry");
	rearPos = carconf.getValue("rearwheels", "position").toVector();

	//Determine wheel heights
	float frontRadius = 0.35, rearRadius = 0.35;
	{
		CBound wheel(NULL);

		wheel.load(frontGeometry, CParamList());
		frontRadius = wheel.m_CylinderRadius;

		wheel.load(rearGeometry, CParamList());
		rearRadius = wheel.m_CylinderRadius;
	}

	CVector hOffset(0.0, 0.5 * (frontRadius-frontPos.y + rearRadius-rearPos.y), 0.0);

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

	//Showroom:
	addObject(
		"misc/showroom.glb", params, CVector(),
		CMatrix(), false, CDataObject::eBound, 0);

	//Body:
	addObject(
		carconf.getValue("body", "geometry"), params, hOffset,
		CMatrix(), true, CDataObject::eBound, 1);

	//Right front wheel
	pos = frontPos + hOffset;
	addObject(frontGeometry, params, pos, mright, true, CDataObject::eBound, 2);

	//Left front wheel
	pos.x = -pos.x;
	addObject(frontGeometry, params, pos, mleft, true, CDataObject::eBound, 3);

	//Right rear wheel
	pos = rearPos + hOffset;
	addObject(rearGeometry, params, pos, mright, true, CDataObject::eBound, 4);

	//Left rear wheel
	pos.x = -pos.x;
	addObject(rearGeometry, params, pos, mleft, true, CDataObject::eBound, 5);
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
