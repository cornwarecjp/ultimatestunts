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
private:

#ifndef _bthread_use_SDL
 pthread_t m_tid;
#else
 SDL_Thread *m_tid;
#endif

public:

bool m_terminate;      // threadFunc() has to return if this is set to true



 bool start()
 {
   #ifdef _bthread_use_SDL
    if (m_tid != NULL) return (false);
    m_tid = SDL_CreateThread(CBThread::run, (void *) this);
    if (m_tid == NULL) return (false);
   #else
    if (m_tid != 0) return (false);      // already running...
    if (pthread_create(&m_tid, NULL, &CBThread::run, (void *) this) != 0) return (false);
   #endif
   return (true);
 }

 bool stop() {
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

 virtual void threadFunc() {}

#ifdef _bthread_use_SDL
static int run(void *arg)
{
   CBThread *parent = (CBThread *) arg;
   parent->threadFunc();
   parent->m_tid = NULL;
   return (0);
}
#else
static void *run(void *arg)
{
   CBThread *parent = (CBThread *) arg;
   parent->threadFunc();
   parent->m_tid = 0;
   return NULL;
 }
#endif


 CBThread() {
   m_terminate = false;

   #ifdef _bthread_use_SDL
    m_tid = NULL;
   #else
    m_tid = 0;
   #endif
 }

 virtual ~CBThread() {
     stop();
  }
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

void enter() {
 #ifdef _bthread_use_SDL
 SDL_mutexP(m_mutex);
 #else
  pthread_mutex_lock(&m_mutex);
 #endif
}

void leave() {
 #ifdef _bthread_use_SDL
  SDL_mutexP(m_mutex);
 #else
  pthread_mutex_unlock(&m_mutex);
 #endif
}

CBCriticalSection() {
#ifdef _bthread_use_SDL
 m_mutex = SDL_CreateMutex();
#else
 pthread_mutex_init(&m_mutex, NULL);
#endif
}

};

#endif
