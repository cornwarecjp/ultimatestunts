/***************************************************************************
                          replayer.cpp  -  Recording and playing replay files
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

#include "netmessages.h"

#include "replayer.h"

#define MAGICHEADER "Ultimate Stunts replay format 0.7.4; network version " USNET_SEARCHFORSTUNTS_VERSION

CReplayer::CReplayer(const CPlayerControl *pctrl)
{
	m_PlayerControl = pctrl;

	//to make sure that we read or write on the first update():
	m_PreviousTime = theWorld->m_LastTime - 1000.0;
}

CReplayer::~CReplayer(){
}

bool CReplayer::open(const CString &filename, bool write)
{
	if(!CDataFile::open(filename, write)) return false;

	bool ret = true;

	if(write)
	{
		if(fillHeaderInfo())
		{
			writeHeader();
		}
		else
		{
			 ret = false;
		}
	}
	else
	{
		if(!readHeader()) ret = false;
	}

	if(!ret)
		close();

	return ret;
}

bool CReplayer::update()
{
	float now = theWorld->m_LastTime;

	if(now < m_PreviousTime - 5.0) //then we obviously started a new game with the same replayer object
		m_PreviousTime = now;

	if(m_Write)
	{
		//if some amount of time has passed:
		if(now >= m_PreviousTime + 0.01) //max. 100 times per second
		{
			m_PreviousTime = now;

			writel(now);
			writeData();
		}
	}
	else
	{
		while(now > m_PreviousTime)
		{
			m_PreviousTime = readl().toFloat();
			if(!readData())
			{
				return false;
			}
		}
	}

	return true;
}

bool CReplayer::fillHeaderInfo()
{
	CTrack *track = theWorld->getTrack();

	if(track == NULL || m_PlayerControl == NULL) return false;

	m_TrackFile = track->getFilename();
	m_ObjectList = m_PlayerControl->getChoiceList();

	return true;
}

void CReplayer::writeHeader()
{
	//magic header
	writel(MAGICHEADER);

	//track file
	writel(m_TrackFile);

	//object choices
	writel((unsigned int)(m_ObjectList.size()));
	for(unsigned int i=0; i < m_ObjectList.size(); i++)
	{
		CBinBuffer b;
		m_ObjectList[i].getData(b);
		writel((unsigned int)(b.size()));
		writeBytes(b);
	}

	/*
	//Object chunk sizes
	m_FrameChunkSize = 0;
	vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);
	for(unsigned int i=0; i < objs.size(); i++)
	{
		CBinBuffer b;
		CMovingObject *mo = (CMovingObject *)objs[i];
		mo->getData(b);
		unsigned int s = b.size();
		m_ObjectChunkSize.push_back(s);
		m_FrameChunkSize += s;
		writel(s);
	}
	*/
}

bool CReplayer::readHeader()
{
	//magic header
	CString line = readl();
	if(line != MAGICHEADER)
		return false;

	//track file
	m_TrackFile = readl();

	//object choices
	unsigned int numObjects = readl().toInt();
	for(unsigned int i=0; i < numObjects; i++)
	{
		CObjectChoice oc;
		unsigned int size = readl().toInt();
		CBinBuffer b = readBytes(size);
		if(b.size() != size) return false;
		unsigned int pos = 0;
		if(!(oc.setData(b, pos))) return false;
		m_ObjectList.push_back(oc);
	}

	/*
	//Object chunk sizes
	m_FrameChunkSize = 0;
	vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);
	for(unsigned int i=0; i < numObjects; i++)
	{
		unsigned int s = readl().toInt();
		m_ObjectChunkSize.push_back(s);
		m_FrameChunkSize += s;
	}
	*/

	return true;
}

void CReplayer::writeData()
{
	//the moving objects:
	vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);

	for(unsigned int i=0; i < objs.size(); i++)
	{
		CBinBuffer header, data;

		CMovingObject *mo = (CMovingObject *)objs[i];
		mo->getData(data);

		header += (Uint16) data.size();

		writeBytes(header);
		writeBytes(data);
	}
}

bool CReplayer::readData()
{
	//the moving objects:
	vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);

	for(unsigned int i=0; i < objs.size(); i++)
	{
		unsigned int pos = 0;

		CBinBuffer header = readBytes(2); //16 bits
		if(header.size() < 2) //reached end of file
			return false;

		unsigned int dataSize = header.getUint16(pos);
		CBinBuffer data = readBytes(dataSize);

		CMovingObject *mo = (CMovingObject *)objs[i];
		mo->m_AllCollisions.clear();

		pos = 0;
		if(!(mo->setData(data, pos)))
			return false;
	}

	return true;
}
