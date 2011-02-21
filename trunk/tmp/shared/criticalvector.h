/***************************************************************************
                          criticalvector.h  -  A CriticalSection vector class
                             -------------------
    begin                : ma jan 17 2005
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
#ifndef CRITICALVECTOR_H
#define CRITICALVECTOR_H

#include <vector>

#include "bthread.h"

template<typename T>
class CCriticalVector : public std::vector<T>, public CBCriticalSection
{
};

#endif

