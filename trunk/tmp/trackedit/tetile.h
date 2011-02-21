/***************************************************************************
                          tetile.h  -  Track Editor specific tile data
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
#ifndef TETILE_H
#define TETILE_H

#include <vector>
namespace std {}
using namespace std;

#include "graphobj.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CTETile : public CGraphObj
{
public:
	CTETile(CDataManager *manager);
	~CTETile();

	virtual bool load(const CString& filename, const CParamList& list);

	CString m_ConfFilename;
	CString m_GLBFilename;
	CString m_Subset;
	CString m_Flags;

	struct SIntVector
	{int x,y,z;};

	struct SRoute
	{
		vector<SIntVector> inPos, outPos;
	};

	vector<SRoute> m_Routes;
	enum eRouteType {eCross, eSplit} m_RouteType;

protected:
	bool loadGLB(const CString &filename, const CParamList &list);
	bool loadConf(const CString &filename, const CParamList &list);

	void loadRoutes();
	void loadRoute(unsigned int i);
};

#endif
