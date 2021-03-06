/***************************************************************************
                          vector.h  -  A 3D vector class
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
#ifndef VECTOR_H
#define VECTOR_H

class CMatrix;

class CVector
{
	public:
		CVector();
		CVector( float xa, float ya, float za);

		//Multiplication with matrix
		CVector const &operator*=(CMatrix const &val);
		//multiplication with semi-inverse matrix
		CVector const &operator/=(CMatrix const &val);

		//scalar multiplication/division
		CVector const operator*(float const &val) const;
		CVector const &operator*=(float const &val);
		CVector const operator/(float const &val) const;
		CVector const &operator/=(float const &val);

		//vector addition/substraction
		CVector const operator+(CVector const &val) const;
		CVector const operator-(CVector const &val) const;
		CVector const &operator+=(CVector const &val);
		CVector const &operator-=(CVector const &val);
		CVector const operator-() const;

		//vector multiplication
		float const operator*(CVector const &val) const; //dot product
		float dotProduct(CVector const &val) const;
		CVector crossProduct(CVector const &val) const;

		float abs() const;
		float abs2() const; //square of abs(); less calculation needed
		void normalise();
		CVector normal() const;
		CVector component(CVector const &val) const; //assumes val is normalised

		float x, y, z; //Don't want to follow the naming conventions!
};

CVector const operator*(float const &val1, CVector const &val2);

#endif
