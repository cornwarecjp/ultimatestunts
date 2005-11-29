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

#include "clientnet.h"
#include "filechunk.h"
#include "datafile.h"

CNetFileControl::CNetFileControl() : CFileControl(false)
{
	m_OriginalFileControl = theFileControl;
	theFileControl = this;
	m_Network = NULL;
}

CNetFileControl::~CNetFileControl()
{
	theFileControl = m_OriginalFileControl;
}

void CNetFileControl::setNetwork(CClientNet *net)
{
	m_Network = net;
}

CString CNetFileControl::getLongFilename(const CString &shortName)
{
	CString localfile = m_OriginalFileControl->getLongFilename(shortName);

	//TODO: check if this is the same file as on the server
	if(localfile != "") return localfile;

	//Just in cases when there is no network available (should not happen here)
	if(m_Network == NULL)
		return "";

	//File transfer over network
	int retID = m_Network->sendTextMessage(CString("GET ") + shortName); //requesting the file
	if(retID > 0) //It has the file
	{
		CFileChunk theChunk;

		printf("Downloading file %s\n", shortName.c_str());

		//Downloading the first chunk
		while(theChunk.m_Filename != shortName)
		{
			//the first chunk, timeout of 5 seconds
			CMessageBuffer *buf = m_Network->receiveExpectedData(CMessageBuffer::fileChunk, 5000);

			if(buf == NULL)
			{
				printf("  Warning: server didn't send file %s\n", shortName.c_str());
				return ""; //server didn't send any chunk
			}

			m_Network->sendConfirmation(*buf, 0);

			theChunk.setBuffer(*buf);
			delete buf;
		}

		//File properties
		unsigned int fileSize = theChunk.m_FileSize;
		unsigned int numChunks = fileSize / FILECHUNK_SIZE;
		if(FILECHUNK_SIZE * numChunks < fileSize) numChunks++; //there is a last, smaller chunk

		//The data buffer
		vector<CBinBuffer> receivedBuffers;
		receivedBuffers.resize(numChunks);
		receivedBuffers[theChunk.m_ChunkID] = theChunk.m_Data;
		unsigned int numReceived = 1;

		//Downloading the other chunks
		while(numReceived < numChunks)
		{
			CMessageBuffer *buf = m_Network->receiveExpectedData(CMessageBuffer::fileChunk, 5000);

			if(buf == NULL)
			{
				printf("  Warning: downloading of file %s has stopped\n", shortName.c_str());
				return ""; //didn't receive any chunks anymore
			}

			m_Network->sendConfirmation(*buf, 0);

			theChunk.setBuffer(*buf);
			delete buf;
			
			if(theChunk.m_Filename != shortName) continue; //not a chunk for this file

			if(receivedBuffers[theChunk.m_ChunkID].size() == 0) //we receive this chunk for the first time
			{
				receivedBuffers[theChunk.m_ChunkID] = theChunk.m_Data;
				numReceived++;
				printf("Downloading %s (%d / %d)\r", shortName.c_str(), numReceived, numChunks);
			}
		}

		printf("Downloading %s finished, saving...\n", shortName.c_str());

		//Saving the file
		CDataFile theFile(shortName, true);
		for(unsigned int i=0; i < receivedBuffers.size(); i++)
			theFile.writeBytes(receivedBuffers[i]);

		return theFile.useExtern();
	}

	return "";
}
