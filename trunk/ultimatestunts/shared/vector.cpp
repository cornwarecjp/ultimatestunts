/***************************************************************************
                          vector.cpp  -  description
                             -------------------
    begin                : Tue Apr 23 2002
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
#include "vector.h"
#include "matrix.h"

CVector::CVector()
{x=0; y=0; z=0;}

CVector::CVector( float xa, float ya, float za)
{
    x = xa;
    y = ya;
    z = za;
}

CVector const &CVector::operator*=(CMatrix const &val)
{
	float xt = x, yt = y, zt = z;

	x = val.Element(0,0)*xt + val.Element(1,0)*yt + val.Element(2,0)*zt;
	y = val.Element(0,1)*xt + val.Element(1,1)*yt + val.Element(2,1)*zt;
	z = val.Element(0,2)*xt + val.Element(1,2)*yt + val.Element(2,2)*zt;

	return (*this);
}

CVector const &CVector::operator/=(CMatrix const &val)
{
	float xt = x, yt = y, zt = z;

	x = val.Element(0,0)*xt + val.Element(0,1)*yt + val.Element(0,2)*zt;
	y = val.Element(1,0)*xt + val.Element(1,1)*yt + val.Element(1,2)*zt;
	z = val.Element(2,0)*xt + val.Element(2,1)*yt + val.Element(2,2)*zt;

	return (*this);
}

CVector const CVector::operator*(float const &val)
{
	float xt = x*val, yt = y*val, zt = z*val;

	CVector ret(xt,yt,zt);

	return ret;
}

CVector const CVector::operator+(CVector const &val)
{
	float xt = x+val.x, yt = y+val.y, zt = z+val.z;

	CVector ret(xt,yt,zt);

	return ret;
}

CVector const CVector::operator-(CVector const &val)
{
	float xt = x-val.x, yt = y-val.y, zt = z-val.z;

	CVector ret(xt,yt,zt);

	return ret;
}

CVector const &CVector::operator+=(CVector const &val)
{
	x += val.x;
	y += val.y;
	z += val.z;

	return (*this);
}
