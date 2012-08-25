/***************************************************************************
                          uploadmanager.cpp  -  Managing the file requests of clients
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

#include "uploadmanager.h"

#include "main.h"

CUploadManager::CUploadManager()
{
	m_ClientSchedule = 0;
}

CUploadManager::~CUploadManager(){
}

int CUploadManager::addFileRequest(unsigned int clientID, const CString &file)
{
	CDataFile f;
	if(!f.open(file)) return 0; //we don't have the file

	SFile sf;
	m_UploadQueue.push_back(sf);
	SFile &FileStruct = m_UploadQueue.back();

	FileStruct.clientID = clientID;
	unsigned int fileSize = 0;
	unsigned int chunkID = 0;
	while(true)
	{
		CFileChunk c;
		c.m_ChunkID = chunkID;
		c.m_Data = f.readBytes(FILECHUNK_SIZE);
		c.m_Filename = file;
		c.m_FileSize = 0; //will be set

		if(c.m_Data.size() == 0) break;

		fileSize += c.m_Data.size();
		chunkID++;
		FileStruct.chunks.push_back(c);
	}

	list<CFileChunk>::iterator i = FileStruct.chunks.begin();
	while(i != FileStruct.chunks.end())
	{
		i->m_FileSize = fileSize;
		i++;
	}

	return 1; //we have the file
}

bool CUploadManager::sendNextChunk()
{
	if(m_UploadQueue.size() == 0) return false;

	if(m_ClientSchedule >= m_UploadQueue.size()) m_ClientSchedule = 0;

	//printf("Sending a file chunk\n");

	SFile &FileStruct = m_UploadQueue[m_ClientSchedule];
	CMessageBuffer b = FileStruct.chunks.front().getBuffer();
	b.setAC(1);
	networkthread.sendToClient(b, FileStruct.clientID);
	FileStruct.chunks.pop_front(); //remove this chunk

	if(FileStruct.chunks.empty()) //then we're ready with this file
		m_UploadQueue.erase(m_UploadQueue.begin() + m_ClientSchedule); //remove this file entry

	m_ClientSchedule++;
	return true;
}
