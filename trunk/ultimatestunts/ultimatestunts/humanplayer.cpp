/***************************************************************************
                          humanplayer.cpp  -  description
                             -------------------
    begin                : Thu Dec 5 2002
    copyright            : (C) 2002 by CJP
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

#include "humanplayer.h"

CHumanPlayer::CHumanPlayer(const CWorld *w) : CPlayer(w)
{}

CHumanPlayer::~CHumanPlayer(){
}

bool CHumanPlayer::Update()
{
	return true;
}
