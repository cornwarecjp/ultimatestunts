/***************************************************************************
                          generalvector.h  -  A vector class to solve matrix equations
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

#ifndef GENERALVECTOR_H
#define GENERALVECTOR_H

#include <vector>
namespace std {}
using namespace std;

/**
  *@author CJP
  */

class CGeneralVector : public vector<float>
{
public:
	CGeneralVector();
	CGeneralVector(unsigned int size);

	float norm() const;
	float dotProduct(const CGeneralVector &val) const;
	float abs2() const;
	float abs() const;
	void normalise();

	CGeneralVector operator*(float val) const;
	CGeneralVector &operator*=(float val);
	CGeneralVector &operator+=(const CGeneralVector &val);
};

#endif
