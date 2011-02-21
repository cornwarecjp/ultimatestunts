/***************************************************************************
                          bthread.cpp  -  bones's thread classes
                             -------------------
    begin                : wo jan 12 2005
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

#include "bthread.h"

CBThread::CBThread()
{
	m_terminate = false;

#ifdef _bthread_use_SDL
	m_tid = NULL;
#else
	m_tid = 0;
#endif
}

CBThread::~CBThread()
{
	stop();
}

bool CBThread::start()
{
#ifdef _bthread_use_SDL
	if (m_tid != NULL) return (false);
	m_terminate = false;
	m_tid = SDL_CreateThread(CBThread::run, (void *) this);
	if (m_tid == NULL) return (false);
#else
	if (m_tid != 0) return (false);      // already running...
	m_terminate = false;
	if (pthread_create(&m_tid, NULL, &CBThread::run, (void *) this) != 0) return (false);
#endif

	return (true);
}

bool CBThread::stop()
{
#ifdef _bthread_use_SDL
	if (m_tid == NULL) return (false);
	m_terminate = true;
	SDL_WaitThread(m_tid, NULL);
	m_tid = NULL;
#else
	if (m_tid == 0) return (false);      // no thread running
	m_terminate = true;
	pthread_join(m_tid, NULL);
	m_tid = 0;
#endif

	return (true);
}

bool CBThread::isRunning()
{
#ifdef _bthread_use_SDL
	return m_tid != NULL;
#else
	return m_tid != 0;
#endif
}

#ifdef _bthread_use_SDL
int CBThread::run(void *arg)
{
	CBThread *parent = (CBThread *) arg;
	parent->threadFunc();
	parent->m_tid = NULL;
	return (0);
}
#else
void *CBThread::run(void *arg)
{
	CBThread *parent = (CBThread *) arg;
	parent->threadFunc();
	parent->m_tid = 0;
	return NULL;
}
#endif


CBCriticalSection::CBCriticalSection()
{
#ifdef _bthread_use_SDL
	m_mutex = SDL_CreateMutex();
#else
	pthread_mutex_init(&m_mutex, NULL);
#endif
}

CBCriticalSection::~CBCriticalSection()
{
#ifdef _bthread_use_SDL
	SDL_DestroyMutex(m_mutex);
#else
	pthread_mutex_destroy(&m_mutex);
#endif
}

void CBCriticalSection::enter()
{
#ifdef _bthread_use_SDL
	SDL_mutexP(m_mutex);
#else
	pthread_mutex_lock(&m_mutex);
#endif
}

void CBCriticalSection::leave()
{
#ifdef _bthread_use_SDL
	SDL_mutexP(m_mutex);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}
