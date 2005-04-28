/***************************************************************************
                          sndsample.h  -  A 3D sound sample class
                             -------------------
    begin                : di feb 25 2003
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

#ifndef SNDSAMPLE_H
#define SNDSAMPLE_H


/**
  *@author CJP
  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBFMOD
#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#endif
#endif

#include "cstring.h"
#include "vector.h"
#include "dataobject.h"

class CSndSample : public CDataObject
{
public:
	CSndSample(CDataManager *manager);
	virtual ~CSndSample();

	virtual bool load(const CString &filename, const CParamList &list);

	virtual int attachToChannel(int c);

protected:

#ifdef HAVE_LIBFMOD
	FSOUND_SAMPLE *m_Sample;
#endif

#ifdef HAVE_LIBOPENAL
	unsigned int m_Buffer;
	bool m_isLoaded;
#endif

};

#endif
