/***************************************************************************
                          sndsample.h  -  description
                             -------------------
    begin                : di feb 25 2003
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

#ifndef SNDSAMPLE_H
#define SNDSAMPLE_H


/**
  *@author CJP
  */

#define SAMPLE_3D 1
#define SAMPLE_2D 2

#include "cstring.h"

class CSndSample
{
  public:
    CSndSample(int t);
    ~CSndSample();

    int loadFromFile(CString filename);

    int attachToChannel(int c);
  protected:
#ifdef HAVE_LIBFMOD
    void *m_Sample;
    int m_Type;
#endif
};

#endif
