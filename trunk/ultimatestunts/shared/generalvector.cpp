/***************************************************************************
                          generalvector.cpp  -  A vector class to solve matrix equations
                             -------------------
    begin                : wo feb 25 2004
    copyright            : (C) 2004 by CJP
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
#include <cmath>

#include "generalvector.h"

CGeneralVector::CGeneralVector()
{
}

CGeneralVector::CGeneralVector(unsigned int size)
{
	for(unsigned int i=0; i < size; i++)
		push_back(0.0);
}

float CGeneralVector::norm() const
{
	float norm = 0.0;
	for(unsigned int i=0; i < size(); i++)
	{
		float n = fabs((*this)[i]);
		if(n > norm) norm = n;
	}
	return norm;

}

float CGeneralVector::dotProduct(const CGeneralVector &val) const
{
	unsigned int s1 = size(), s2 = val.size();
	unsigned int max = s1>s2? s1 : s2;
	float ret = 0.0;
	for(unsigned int i=0; i < max; i++)
		ret += (*this)[i] * val[i];

	return ret;
}

float CGeneralVector::abs2() const
{
	float ret = 0.0;
	for(unsigned int i=0; i < size(); i++)
		ret += (*this)[i] * (*this)[i];
	return ret;
}

float CGeneralVector::abs() const
{
	return sqrt(abs2());
}

void CGeneralVector::normalise()
{
	float s = abs();
	for(unsigned int i=0; i < size(); i++)
		(*this)[i] /= s;
}

CGeneralVector CGeneralVector::operator*(float val) const
{
	CGeneralVector ret;

	for(unsigned int i=0; i < size(); i++)
		ret.push_back((*this)[i] * val);

	return ret;
}

CGeneralVector &CGeneralVector::operator*=(float val)
{
	for(unsigned int i=0; i < size(); i++)
		(*this)[i] *= val;

	return *this;
}
