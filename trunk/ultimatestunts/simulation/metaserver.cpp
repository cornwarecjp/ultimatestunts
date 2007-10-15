/***************************************************************************
                          metaserver.cpp  -  Connect to a metaserver for finding game servers
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
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "lconfig.h"

#include "netmessages.h"
#include "metaserver.h"

CMetaServer::CMetaServer()
{
	m_Hostname = theMainConfig->getValue("files", "metaserver_hostname");
	m_URI = theMainConfig->getValue("files", "metaserver_filename");
	m_Port = theMainConfig->getValue("files", "metaserver_port").toInt();

	m_Connected = false;
}


CMetaServer::~CMetaServer()
{
	if(m_Connected) tcpDisconnect();
}


//Public API:
vector<CClientNet::SBroadcastResult> CMetaServer::getServerList()
{
	printf("getServerList()\n");

	vector<CClientNet::SBroadcastResult> ret;

	if(!tcpConnect()) return ret; //TODO: show error message
	if(!httpGet("")) return ret; //TODO: show error message
	CString response = httpReceiveResponse();
	tcpDisconnect();

	printf("%s\n", response.c_str());

	CClientNet::SBroadcastResult s;

	while(response.length() > 0 && getLineFromStr(response) != "START"); //Search for START mark
	printf("Found START\n");

	//Process entries
	while(response.length() > 0)
	{
		CString version = getLineFromStr(response);
		if(version == "END") break;
		printf("version = %s\n", version.c_str());

		if(version == USNET_SEARCHFORSTUNTS_VERSION) //This entry has the right protocol version
		{
			//printf("version = %s\n", version.c_str());

			//Now read information
			s.serverName = getLineFromStr(response);
			printf("servername = %s\n", s.serverName.c_str());
			s.hostName = getLineFromStr(response);
			printf("hostname = %s\n", s.hostName.c_str());
			s.port = getLineFromStr(response).toInt();
			printf("port = %d\n", s.port);
			ret.push_back(s);
		}

		while(getLineFromStr(response) != ""); //skip all lines until an empty line
	}

	return ret;
}

bool CMetaServer::setServer(const CString &name, unsigned int port)
{

	printf(
		"Setting server: name %s, port %d\n",
		name.c_str(), port);

	CString version = USNET_SEARCHFORSTUNTS_VERSION;
	CString servername = name;
	urlEncode(version);
	urlEncode(servername);

	if(!tcpConnect()) return false;
	if(!httpPost("",
		CString("setServer=1")+
		"&version="+version+
		"&name="+servername+
		"&port="+port
		)) return false;
	CString response = httpReceiveResponse();
	tcpDisconnect();

	printf("%s\n", response.c_str());

	return true;
}

void CMetaServer::removeServer(unsigned int port)
{
	printf("Removing server\n");

	if(!tcpConnect()) return;
	if(!httpPost("",
		CString("removeServer=1")+
		"&port="+port
		)) return;
	CString response = httpReceiveResponse();
	tcpDisconnect();

	printf("%s\n", response.c_str());
}

CString CMetaServer::getLineFromStr(CString &buffer)
{
	CString ret;
	int pos = buffer.inStr('\n');
	if(pos < 0)
	{
		ret = buffer;
		buffer = "";
		return ret;
	}

	ret = buffer.mid(0, pos);
	buffer = buffer.mid(pos+1);

	//Remove \r
	if(ret[ret.length()-1] == '\r')
		ret = ret.mid(0, ret.length()-1);

	//printf("Found line: \"%s\"\n", ret.c_str());

	return ret;
}

void CMetaServer::urlEncode(CString &str)
{
	for(unsigned int i=0; i < str.length(); i++)
	{
		//Alphanumeric
		if(
			(str[i] >= 'A' && str[i] <= 'Z') ||
			(str[i] >= 'a' && str[i] <= 'z') ||
			(str[i] >= '0' && str[i] <= '9'))
			continue; //leave unchanged

		//Space
		if(str[i] == ' ')
			{str[i] = '+'; continue;}

		//Default: encode with %xx notation
		CString prev = str.mid(0, i) + CString().format("%%%x", 8, str[i]);
		str = prev + str.mid(i+1);
		i = prev.length()-1;
	}
}

//HTTP-level
bool CMetaServer::httpGet(const CString &args)
{
	return writeStr(
		CString("GET ") + m_URI+args + " HTTP/1.1\r\n"
		"Host: " + m_Hostname + "\r\n"
		"Connection: close\r\n"
		"User-Agent: " + PACKAGE + "/" + VERSION + "\r\n"
		"\r\n"
		);
}

bool CMetaServer::httpPost(const CString &args, const CString &data)
{
	CString postdata =
		CString("POST ") + m_URI+args + " HTTP/1.1\r\n"
		"Host: " + m_Hostname + "\r\n"
		"Connection: close\r\n"
		"User-Agent: " + PACKAGE + "/" + VERSION + "\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: " + (unsigned int)data.length() + "\r\n"
		"\r\n"
		+ data + "\r\n";

	return writeStr(postdata);
}

CString CMetaServer::httpReceiveResponse()
{
	httpReadHeaders();

	if(m_IsChunked)
		return httpReadBodyChunked();

	return httpReadBodyNormal();
}

CString CMetaServer::httpReadBodyChunked()
{
	int dataLength;
	CString lenStr, data, ret;

	while(true)
	{
		//printf("\n\nNEW CHUNK\n\n");

		lenStr = readLine();
		if(lenStr == "")
		{
			fprintf(stderr, "ERROR: expected new chunk length line\n");
			return "";
		}

		//printf("lenStr = \"%s\"\n", lenStr);

		dataLength = strtol(lenStr.c_str(), NULL, 16);

		if(dataLength <= 0) break;

		//printf("Reading %d bytes\n", dataLength);
		m_DataLength = dataLength;
		ret += httpReadBodyNormal();

		//Read following CRLF
		if(readChar() == '\n') continue; //LF instead of CRLF
		if(readChar() != '\n')
			printf("ERROR: expected CRLF after data chunk\n");
	}

	return ret;
}

CString CMetaServer::httpReadBodyNormal()
{
	int readLen, remainLen;
	char *data, *writePtr;

	data = (char *)malloc(m_DataLength*sizeof(char));
	writePtr = data;
	remainLen = m_DataLength;

	while(remainLen > 0)
	{
		readLen = read(m_SockFD, writePtr, remainLen);

		if(readLen < 0)
		{
			printf("ERROR reading from socket\n");
			free(data);
			return "";
		}

		writePtr += readLen;
		remainLen -= readLen;
	}

	CString ret;
	for(unsigned i=0; i < m_DataLength; i++)
		ret += data[i];

	free(data);
	return ret;
}

bool CMetaServer::httpReadHeaders()
{
	CString headername;
	CString line;

	m_DataLength = 0;
	m_IsChunked = false;

	//printf("httpReadHeaders()\n");

	while(true)
	{
		line = readLine();
		if(line.mid(0,7) != "HTTP/1.")
			return  false;

		switch(line[9])
		{
		case '1':
			printf("Got 1xx line: %s\n", line.c_str());

			//Ignore rest of this header until empty line
			while(line != "")
				line = readLine();

			continue;
		case '2':
			break;
		default:
			printf("Server returned error: %s\n", line.mid(9).c_str());
			return false;
		}

		break;
	}

	while(true)
	{
		line = httpReadHeaderLine(headername);
		line.toLower();

		if(headername == "")
			break;

		//printf("Header value \"%s\" = \"%s\"\n", headername.c_str(), line.c_str());

		if(headername == "Content-Length")
		{
			if(m_IsChunked)
			{
				printf("WARNING: found both content length and chunked transfer\n");
			}
			else
			{
				m_DataLength = line.toInt();
				//printf("Data length = %d\n", m_DataLength);
			}
		}
		else if(headername == "Connection")
		{
			if(line != "close")
			{
				printf("ERROR: Connection = \"%s\", should be \"close\" "
					"(Persistent connections are not supported)\n", line.c_str());
				return false;
			}
		}
		else if(headername == "Transfer-Encoding")
		{
			if(line == "chunked")
			{
				if(m_DataLength > 0)
					printf("WARNING: found both content length and chunked transfer\n");

				m_IsChunked = true;
			}
			else
			{
				printf("WARNING: Transfer-encoding not recognized: %s\n", line.c_str());
			}
		}
		/*
		else
		{
			printf("Ignored header value \"%s\" = \"%s\"\n", headername, line);
		}
		*/
	}

	return true;
}

CString CMetaServer::httpReadHeaderLine(CString &name)
{
	CString line = readLine();

	//Default name:
	name = "_";

	//printf("  Read header line: %s\n", line);

	if(line == "")
	{
		name = "";  //empty line
		return "";
	}

	int pos = line.inStr(':');
	if(pos < 0)
	{
		printf("WARNING: HTTP header line does not contain a \':\': %s\n", line.c_str());

		//Don't consider this to be an error
		return "";
	}

	name = line.mid(0, pos);

	pos++;
	while(true)
	{
		if((unsigned int)pos >= line.length())
		{
			printf("WARNING: HTTP header line does not contain a value: %s\n", line.c_str());

			//Don't consider this to be an error
			return "";
		}

		if(line[pos] != ' ' && line[pos] != '\t') break;

		pos++;
	}

	return line.mid(pos);
}



//Read-and-write-utilities
CString CMetaServer::readLine()
{
	CString ret;

	while(true)
	{
		int c = readChar();
		if(c == '\n' || c < 0) break; //stop
		if(c == '\r') continue; //ignore

		ret += (unsigned char)c;
	}

	//printf("%s\n", ret.c_str());

	return ret;
}

int CMetaServer::readChar()
{
	int n;
	unsigned char ret;
	n = read(m_SockFD, &ret, 1);

	if(n == 0)
	{
		printf("Connection closed\n");
		return -1;
	}
	else if (n < 0)
	{
		printf("ERROR reading from socket\n");
		return -1;
	}

	//printf("\'%c\'\n", ret);

	return ret;
}

CString CMetaServer::readStr(unsigned int maxlen)
{
	int n;
	unsigned char buffer[maxlen];
	n = read(m_SockFD, buffer, maxlen-1);

	if(n == 0)
	{
		printf("Connection closed\n");
	}
	else if (n < 0)
	{
		printf("ERROR reading from socket\n");
		return "";
	}

	/* Make zero-terminated */
	buffer[n] = '\0';
	return CString(buffer);
}

bool CMetaServer::writeStr(const CString &data)
{
	//printf("%s\n", data.c_str());

	int n = write(m_SockFD, data.c_str(), data.length());
	if (n < 0)
	{
		printf("ERROR writing to socket\n");
		return false;
	}

	return true;
}


//TCP-level
bool CMetaServer::tcpConnect()
{
	//Disconnect previous connection
	if(m_Connected) tcpDisconnect();

	struct sockaddr_in serv_addr;
	struct hostent *server;

	m_SockFD = socket(AF_INET, SOCK_STREAM, 0);
	if (m_SockFD < 0)
	{
		printf("ERROR opening socket\n");
		return false;
	}

	server = gethostbyname(m_Hostname.c_str());
	if (server == NULL)
	{
		printf("ERROR, no such host\n");
		return false;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(m_Port);

	if (connect(m_SockFD,(struct sockaddr *)(&serv_addr),sizeof(serv_addr)) < 0)
	{
		printf("ERROR connecting\n");
		return false;
	}

	m_Connected = true;
	return true;
}

void CMetaServer::tcpDisconnect()
{
	close(m_SockFD);

	m_Connected = false;
}

