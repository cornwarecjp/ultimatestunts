/***************************************************************************
                          matrix.h  -  description
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
		CMatrix(CVector v);
		CMatrix(CMatrix const &val);
		~CMatrix();

		CMatrix const &operator=(CMatrix const &val);
		CMatrix const &operator*=(CMatrix const &val);

		void  rotY ( float hoek);
		void  rotX ( float hoek);
    void reset();

		float Element(int i, int j) const;
		void setElement(int i, int j, float e);

	protected:
		float *m_M;
};

#endif
