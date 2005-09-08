/***************************************************************************
                          netfilecontrol.cpp  -  Automatic downloading + uploading of data files
                             -------------------
    begin                : di aug 30 2005
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

#include <cstdio>

#include "netfilecontrol.h"

CNetFileControl::CNetFileControl() : CFileControl(false)
{
	m_OriginalFileControl = theFileControl;
	theFileControl = this;
}

CNetFileControl::~CNetFileControl()
{
	theFileControl = m_OriginalFileControl;
}

CString CNetFileControl::getLongFilename(const CString &shortName)
{
	CString localfile = m_OriginalFileControl->getLongFilename(shortName);

	//TODO: check if they are the same files
	if(localfile != "") return localfile;

	//TODO: file transfer over network

	return "";
}
