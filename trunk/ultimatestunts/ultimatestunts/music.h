/***************************************************************************
                          music.h  -  A Music class
                             -------------------
    begin                : wo aug 13 2003
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

#ifndef MUSIC_H
#define MUSIC_H

/**
  *@author CJP
  */

#include <vector>
namespace std {}
using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBFMOD
#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#endif
#endif

#ifdef HAVE_VORBISFILE
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#endif

#include "cstring.h"
#include "sndsample.h"
#include "usmacros.h"

class CMusic : public CSndSample
{
  public:
	CMusic(CDataManager *manager);
	virtual ~CMusic();

	virtual bool load(const CString &filename, const CParamList &list);

	virtual int attachToChannel(int c);

	void update();

	void setEndCallback(void (CALLBACKFUN *endfunc)());

protected:

#ifdef HAVE_LIBFMOD
	FSOUND_STREAM *m_Stream;
#endif

#ifdef HAVE_LIBOPENAL
	unsigned int m_Buffer;
	unsigned int m_Source; //only used when streaming
	bool m_isLoaded;
	bool m_isStreaming;
	bool m_streamIsFinished;
	vector<unsigned int> m_StreamBuffers;

#ifdef HAVE_VORBISFILE
	OggVorbis_File m_VorbisFile;
#endif

#endif
};

#endif
