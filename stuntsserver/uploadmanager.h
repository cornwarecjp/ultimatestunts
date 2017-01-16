/***************************************************************************
                          uploadmanager.h  -  Managing the file requests of clients
                             -------------------
    begin                : ma nov 28 2005
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

#ifndef UPLOADMANAGER_H
#define UPLOADMANAGER_H

#include <list>
namespace std {}
using namespace std;

#include "cstring.h"
#include "filechunk.h"

/**
  *@author CJP
  */

class CUploadManager {
public: 
	CUploadManager();
	~CUploadManager();

	int addFileRequest(unsigned int clientID, const CString &file);

	bool sendNextChunk();
protected:

	struct SFile
	{
		unsigned int clientID;
		list<CFileChunk> chunks;
	};

	vector<SFile> m_UploadQueue;
	unsigned int m_ClientSchedule;
};

#endif
