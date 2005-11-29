/***************************************************************************
                          netfilecontrol.h  -  Automatic downloading + uploading of data files
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

#ifndef NETFILECONTROL_H
#define NETFILECONTROL_H

#include "filecontrol.h"

/**
  *@author CJP
  */

class CClientNet;
  
class CNetFileControl : protected CFileControl  {
public: 
	CNetFileControl();
	~CNetFileControl();

	void setNetwork(CClientNet *net);

	CString getLongFilename(const CString &shortName); //for reading only!

private:
	CFileControl *m_OriginalFileControl;

	CClientNet *m_Network;
};

#endif
