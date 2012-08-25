/***************************************************************************
                          hiscore.cpp  -  Hiscore of a race, or a track
                             -------------------
    begin                : do nov 24 2005
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

#include "hiscore.h"

CHiscore::CHiscore(){
}
CHiscore::~CHiscore(){
}

CBinBuffer &CHiscore::getData(CBinBuffer &b) const
{
	b += (Uint8)size();

	for(unsigned int i=0; i < size(); i++)
	{
		SHiscoreEntry ent = operator[](i);

		b += ent.name;
		b += ent.carname;
		//b += (Uint8)(ent.isNew); //when sent over a network, they're always new
		b.addFloat32(ent.time, 0.005); //more accurate than 1/100 sec
	}

	return b;
}

bool CHiscore::setData(const CBinBuffer &b, unsigned int &pos)
{
	clear();

	unsigned int s = b.getUint8(pos);

	for(unsigned int i=0; i < s; i++)
	{
		SHiscoreEntry ent;

		ent.name = b.getCString(pos);
		ent.carname = b.getCString(pos);
		ent.isNew = true; //bool(b.getUint8(pos)); //when sent over a network, they're always new
		ent.time = b.getFloat32(pos, 0.005);

		push_back(ent);
	}

	return true;
}

