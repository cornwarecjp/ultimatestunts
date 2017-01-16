/***************************************************************************
                          bthread.h  - bones's thread classes
                             -------------------
    begin                : Sun Sep 1 2002
    copyright            : (C) 2002 by bones
    email                : martin@euraf.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef bthread_H
#define bthread_H

// #define _bthread_use_SDL

#ifndef _bthread_use_SDL
#include <pthread.h>
#else
#include "SDL_thread.h"
#endif

class CBThread {
public:
	CBThread();
	virtual ~CBThread();

	bool m_terminate;      // threadFunc() has to return if this is set to true

	bool start();
	bool stop();
	bool isRunning();

	virtual void threadFunc()=0;

#ifdef _bthread_use_SDL
	static int run(void *arg);
#else
	static void *run(void *arg);
#endif

private:

#ifndef _bthread_use_SDL
	pthread_t m_tid;
#else
	SDL_Thread *m_tid;
#endif

};


//
// bcriticalsection
//
class CBCriticalSection {

#ifdef _bthread_use_SDL
	SDL_mutex *m_mutex;
#else
	pthread_mutex_t m_mutex;
#endif

public:
	CBCriticalSection();
	~CBCriticalSection();

	void enter();
	void leave();
};

#endif
