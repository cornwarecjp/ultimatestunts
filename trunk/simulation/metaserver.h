/***************************************************************************
                          metaserver.h  -  Connect to a metaserver for finding game servers
                             -------------------
    begin                : do jul 5 2007
    copyright            : (C) 2007 by CJP
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
#ifndef METASERVER_H
#define METASERVER_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "cstring.h"

#include "ipnumber.h"
#include "clientnet.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CMetaServer {
public:
	CMetaServer();
	~CMetaServer();

	//Public API:
	vector<CClientNet::SBroadcastResult> getServerList();
	bool setServer(const CString &name, unsigned int port); //add or change
	void removeServer(unsigned int port);

protected:
	CString m_Hostname, m_URI;
	unsigned int m_Port;

	unsigned int m_SockFD;
	bool m_Connected;

	bool m_IsChunked;
	unsigned int m_DataLength;

	CString getLineFromStr(CString &buffer);
	void urlEncode(CString &str);

	//HTTP-level
	bool httpGet(const CString &args);
	bool httpPost(const CString &args, const CString &data);

	CString httpReceiveResponse();
	CString httpReadBodyChunked();
	CString httpReadBodyNormal();
	bool httpReadHeaders();
	CString httpReadHeaderLine(CString &name);

	//Read-and-write-utilities
	CString readLine();
	int readChar();
	CString readStr(unsigned int maxlen);
	bool writeStr(const CString &data);

	//TCP-level
	bool tcpConnect();
	void tcpDisconnect();
};

#endif
