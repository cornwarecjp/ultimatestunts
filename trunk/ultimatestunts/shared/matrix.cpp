/***************************************************************************
                          matrix.cpp  -  A 3D rotation matrix class
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
#include "matrix.h"
//#include <stdio.h>
#include <math.h>

CMatrix::CMatrix()
{
    m_M = new float[9];

    reset();
}

CMatrix::CMatrix(CVector v)
{
	m_M = new float[9];

	//This function is NOT finished.
	//It currently only handles rotations around the y-axis.
	rotY(v.y);
}

CMatrix::CMatrix(CMatrix const &val)
{
	m_M = new float[9];
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
			*(m_M + i + 3*j) = val.Element(i,j);
}

CMatrix::~CMatrix()
{
	delete [] m_M;
}

CMatrix const &CMatrix::operator=(CMatrix const &val)
{

	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
			*(m_M + i + 3*j) = val.Element(i,j);

	return (*this);
}

CMatrix const &CMatrix::operator*=(CMatrix const &val)
{
  CMatrix temp = *this;

	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
			temp.setElement(i,j,
        *(m_M + i    ) * val.Element(0,j) +
        *(m_M + i + 3) * val.Element(1,j) +
        *(m_M + i + 6) * val.Element(2,j)
      );

  *this = temp;

	return (*this);
}

float CMatrix::Element(int i, int j) const
{
	return *(m_M + i + 3*j);
}

void CMatrix::setElement(int i, int j, float e)
{
	*(m_M + i + 3*j) = e;
}

void  CMatrix::rotY ( float hoek)
{
    *(m_M+0)=cos(hoek);  *(m_M+1)=0;  *(m_M+2)=-sin(hoek);
    *(m_M+3)=0;          *(m_M+4)=1;  *(m_M+5)=0;
    *(m_M+6)=sin(hoek); *(m_M+7)=0;  *(m_M+8)=cos(hoek);
}

void  CMatrix::rotX ( float hoek)
{
    *(m_M+0)=1; *(m_M+1)=0;         *(m_M+2)=0;
    *(m_M+3)=0; *(m_M+4)=cos(hoek); *(m_M+5)=-sin(hoek);
    *(m_M+6)=0; *(m_M+7)=sin(hoek); *(m_M+8)=cos(hoek);
}

void CMatrix::reset()
{
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            if (i==j)
                *(m_M + i + 3*j)=1;
            else
                *(m_M + i + 3*j)=0;
        }
    }
}
