/***************************************************************************
                          tetile.cpp  -  Track Editor specific tile data
                             -------------------
    begin                : za mrt 29 2008
    copyright            : (C) 2008 by CJP
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

#include "datafile.h"
#include "lconfig.h"
#include "vector.h"

#include "tetile.h"

CTETile::CTETile(CDataManager *manager) : CGraphObj(manager, eTileModel)
{
	m_RouteType = eCross;
}


CTETile::~CTETile()
{
}


bool CTETile::load(const CString& filename, const CParamList& list)
{
	bool ret = false;

	if(filename.mid(filename.length()-4) == ".glb")
	{
		m_Subset = list.getValue("subset", "");
		m_Flags = list.getValue("flags", "");
		m_GLBFilename = filename;
		m_ConfFilename = "";
		ret = loadGLB(filename, list);
	}
	else
	{
		//Default for other extensions: .conf loader
		m_ConfFilename = filename;
		ret = loadConf(filename, list);
	}

	//Fix final CDataObject state:
	m_Filename = filename;

	return ret;
}

CLConfig *_TETile_conffile = NULL;

bool CTETile::loadConf(const CString &filename, const CParamList &list)
{
	CDataFile df(filename);
	CLConfig file(df.useExtern());

	m_GLBFilename = file.getValue("model", "glbfile");
	if(m_GLBFilename == "")
	{
		printf("Error: conf file \"%s\" does not contain a model\\glbfile value\n",
			filename.c_str());
		return false;
	}

	CString textures = file.getValue("model", "textures");
	m_Subset = m_DataManager->loadFilesFromString(CDataObject::eMaterial, textures);

	m_Flags = file.getValue("model", "flags");

	printf("Conf file = %s\n", filename.c_str());
	printf("subset = %s\n", m_Subset.c_str());
	printf("flags = %s\n", m_Flags.c_str());

	_TETile_conffile = &file;
	loadRoutes();

	return loadGLB(m_GLBFilename, list);
}

bool CTETile::loadGLB(const CString &filename, const CParamList &list)
{
	//New list with modified parameters:
	CParamList newList = list;

	//Make sure the subset parameter equals m_Subset
	newList.setValue("subset", m_Subset);

	bool ret = CGraphObj::load(filename, newList);

	//Restore old list:
	m_ParamList = list;

	m_ParamList.setValue("flags", m_Flags);

	return ret;
}

void CTETile::loadRoutes()
{
	CString type = _TETile_conffile->getValue("routes", "type");
	if(type == "split") m_RouteType = eSplit;

	unsigned int numRoutes = _TETile_conffile->getValue("routes", "number").toInt();
	m_Routes.resize(numRoutes);

	if(numRoutes != 0)
	for(unsigned int i=0; i < numRoutes; i++)
		loadRoute(i);
}

void CTETile::loadRoute(unsigned int i)
{
	CString section = CString("route") + i;
	SRoute &route = m_Routes[i];

	//printf("using section \"%s\"\n", section.c_str());

	unsigned int num_in = _TETile_conffile->getValue(section, "in_number").toInt();
	route.inPos.resize(num_in);

	unsigned int num_out = _TETile_conffile->getValue(section, "out_number").toInt();
	route.outPos.resize(num_out);

	if(num_in != 0)
	for(unsigned int j=0; j < num_in; j++)
	{
		CString field = CString("in_") + j;
		CVector value = _TETile_conffile->getValue(section, field).toVector();

		route.inPos[j].x = (int)roundf(value.x);
		route.inPos[j].y = (int)roundf(value.y);
		route.inPos[j].z = (int)roundf(value.z);
	}

	if(num_out != 0)
	for(unsigned int j=0; j < num_out; j++)
	{
		CString field = CString("out_") + j;
		CVector value = _TETile_conffile->getValue(section, field).toVector();

		route.outPos[j].x = (int)roundf(value.x);
		route.outPos[j].y = (int)roundf(value.y);
		route.outPos[j].z = (int)roundf(value.z);
	}
}

