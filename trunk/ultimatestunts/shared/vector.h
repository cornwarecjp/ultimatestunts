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

    //CVector addition
		CVector const &operator+=(CVector const &val);
    //Multiplication with matrix
		CVector const &operator*=(CMatrix const &val);
    //multiplication with semi-inverse matrix
		CVector const &operator/=(CMatrix const &val);
    //CVector/scalar multiplication
		CVector const operator*(float const &val);
    //CVector addition/substraction
		CVector const operator+(CVector const &val);
		CVector const operator-(CVector const &val);

		float x, y, z; //Don't want to follow the naming conventions!
};

#endif
