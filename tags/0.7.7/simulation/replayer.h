/***************************************************************************
                          replayer.h  -  Recording and playing replay files
                             -------------------
    begin                : do dec 15 2005
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

#ifndef REPLAYER_H
#define REPLAYER_H

#include "simulation.h"
#include "datafile.h"

#include "cstring.h"
#include "playercontrol.h"

/**
  *@author CJP
  */

class CReplayer : public CSimulation, protected CDataFile
{
public: 
	CReplayer(const CPlayerControl *pctrl);
	virtual ~CReplayer();

	bool open(const CString &filename, bool write=false);

	void close() {CDataFile::close();}

	virtual bool update();

	//Header information:
	CString m_TrackFile;
	vector<CObjectChoice> m_ObjectList;

protected:
	bool fillHeaderInfo();
	void writeHeader();
	bool readHeader();
	void writeData();
	bool readData();

	//Recording data
	const CPlayerControl *m_PlayerControl;
	float m_PreviousTime;
};

#endif
