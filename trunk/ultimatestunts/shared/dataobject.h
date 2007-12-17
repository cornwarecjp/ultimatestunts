/***************************************************************************
                          dataobject.h  -  Object loaded from a data file
                             -------------------
    begin                : wo dec 1 2004
    copyright            : (C) 2004 by CJP
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

#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <vector>
namespace std {}
using namespace std;

#include "cstring.h"

/**
  *@author CJP
  */

class CDataManager;

struct SParameter
{
	CString name, value;
};

class CParamList : public vector<SParameter>
{
public:
	CParamList();
	CParamList(const CString &paramline);
	~CParamList();

	bool operator==(const CParamList &val) const;
	CString getValue(const CString &var, const CString &deflt) const;
};

class CDataObject {
public: 
	enum eDataType
	{
		eNone=0,
		eTrack,
		eCollisionModel,
		eBound,
		eTileModel,
		eMaterial,
		eMovingObject,
		eGraphObj,
		eTexture,
		eSample,
		eEnumTop
	};

	CDataObject(CDataManager *manager, eDataType type);
	virtual ~CDataObject();
	
	virtual bool load(const CString &filename, const CParamList &list);
	virtual void unload();

	bool isLoaded()const {return m_isLoaded;}
	CString getFilename() const {return m_Filename;}
	const CParamList &getParamList() const {return m_ParamList;}
	eDataType getType() const {return m_DataType;}

protected:
	CDataManager *m_DataManager;

	bool m_isLoaded;
	CString m_Filename;
	CParamList m_ParamList;
	eDataType m_DataType;
};


//you'll need this class in derived classes
#include "datamanager.h"

#endif
