/***************************************************************************
                          load3ds.h  -  3DS file loader
                             -------------------
    begin                : wo feb 1 2006
    copyright            : (C) 2006 by CJP
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

#ifndef LOAD3DS_H
#define LOAD3DS_H

#include "editgraphobj.h"

bool load3DS(const CString &filename, CEditGraphObj &obj);

#endif
