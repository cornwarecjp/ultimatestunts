/***************************************************************************
                          matrix.h  -  A 3D rotation matrix class
                             -------------------
    begin                : Fri Jul 6 2001
    copyright            : (C) 2001 by CJP
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
#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

class CMatrix
{
	public:
		CMatrix();
		//CMatrix(CVector v); //use setRotation instead
		CMatrix(CMatrix const &val);
		~CMatrix();
		void reset();
		void setCrossProduct(CVector v);
		void setRotation(CVector v);

		//Matrix-operations:
		CMatrix const &operator=(CMatrix const &val);
		CMatrix const &operator*=(CMatrix const &val);
		CMatrix const &operator/=(CMatrix const &val);
		CMatrix operator*(CMatrix const &val) const;
		CMatrix transpose() const;

		//Multiplication with a vector
		CVector const operator*(CVector val) const;
		CVector const operator/(CVector val) const;

		//Multiplication with a scalar
		CMatrix const operator*(float val) const;
		CMatrix const operator/(float val) const;


		//Special ways to create the right rotation matrix:
		void  rotX ( float hoek);
		void  rotY ( float hoek);
		void  rotZ ( float hoek);
		void targetZ(CVector d, bool normalize=true);

		float Element(int i, int j) const;
		void setElement(int i, int j, float e);
		const float *gl_mtr() const;

	protected:
		float *m_M;
};

#endif
