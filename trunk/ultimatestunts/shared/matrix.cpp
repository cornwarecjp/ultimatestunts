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
#include <cmath>

//#include <cstdio>
//#include "cstring.h" //debugging

CMatrix::CMatrix()
{
    m_M = new float[16];
    reset();
}

CMatrix::CMatrix(CMatrix const &val)
{
	m_M = new float[16];
	operator=(val);
}

CMatrix::~CMatrix()
{
	delete [] m_M;
}

const float *CMatrix::gl_mtr() const
{return m_M;}

CMatrix const &CMatrix::operator=(CMatrix const &val)
{

	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++)
			setElement(i,j, val.Element(i,j) );

	return (*this);
}

CMatrix const &CMatrix::operator*=(CMatrix const &val)
{
  CMatrix temp = *this;

	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
			temp.setElement(i,j,
				Element(i,0) * val.Element(0,j) +
				Element(i,1) * val.Element(1,j) +
				Element(i,2) * val.Element(2,j)
			);

	*this = temp;
	return (*this);
}

CMatrix const &CMatrix::operator/=(CMatrix const &val)
{
	CMatrix temp = *this;

	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
			temp.setElement(i,j,
				Element(i,0) * val.Element(j,0) +
				Element(i,1) * val.Element(j,1) +
				Element(i,2) * val.Element(j,2)
			);

	*this = temp;
	return (*this);
}

CVector const CMatrix::operator*(CVector val) const
{
	return CVector(
		Element(0,0)*val.x + Element(1,0)*val.y + Element(2,0)*val.z,
		Element(0,1)*val.x + Element(1,1)*val.y + Element(2,1)*val.z,
		Element(0,2)*val.x + Element(1,2)*val.y + Element(2,2)*val.z
		);
}

CVector const CMatrix::operator/(CVector val) const
{
	return CVector(
		Element(0,0)*val.x + Element(0,1)*val.y + Element(0,2)*val.z,
		Element(1,0)*val.x + Element(1,1)*val.y + Element(1,2)*val.z,
		Element(2,0)*val.x + Element(2,1)*val.y + Element(2,2)*val.z
		);
}

CMatrix const CMatrix::operator*(float val) const
{
	CMatrix ret = *this;
	for(unsigned int i=0; i<2; i++)
		for(unsigned int j=0; j<2; j++)
			ret.setElement(i, j, ret.Element(i,j) * val);

	return ret;
}

CMatrix const CMatrix::operator/(float val) const
{
	return (*this) * (1.0/val);
}

CMatrix CMatrix::operator*(CMatrix const &val) const
{
	CMatrix temp;

	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
			temp.setElement(i,j,
        Element(i,0) * val.Element(0,j) +
        Element(i,1) * val.Element(1,j) +
        Element(i,2) * val.Element(2,j)
      );

	return temp;
}

float CMatrix::Element(int i, int j) const
{
	return *(m_M + 4*i + j);
}

void CMatrix::setElement(int i, int j, float e)
{
	*(m_M + 4*i + j) = e;
}

void  CMatrix::rotZ ( float hoek)
{
	setElement(0,0,cos(hoek));	setElement(1,0,-sin(hoek));	setElement(2,0,0.0);
	setElement(0,1,sin(hoek));	setElement(1,1,cos(hoek));	setElement(2,1,0.0);
	setElement(0,2,0.0);		setElement(1,2,0.0);		setElement(2,2,1.0);
}

void  CMatrix::rotY ( float hoek)
{
	setElement(0,0,cos(hoek));	setElement(1,0,0.0);	setElement(2,0,-sin(hoek));
	setElement(0,1,0.0);		setElement(1,1,1.0);	setElement(2,1,0.0);
	setElement(0,2,sin(hoek));	setElement(1,2,0.0);	setElement(2,2,cos(hoek));
}

void  CMatrix::rotX ( float hoek)
{
	setElement(0,0,1.0);	setElement(0,1,0.0);		setElement(0,2,0.0);
	setElement(1,0,0.0);	setElement(1,1,cos(hoek));	setElement(1,2,-sin(hoek));
	setElement(2,0,0.0);	setElement(2,1,sin(hoek));	setElement(2,2,cos(hoek));
}

void CMatrix::targetZ(CVector d, bool normalize)
{
	if(normalize)
		d /= d.abs();

	float sq =  sqrt(d.x*d.x+d.z*d.z);

	if(sq < 0.01) //prevent division by zero
	{
		reset();
		setElement(1,1, 0.0);		setElement(2,1, 1.0);
		setElement(1,2, -1.0);		setElement(2,2, 0.0);
		return;
	}

	float invsqrt = 1.0 / sq;

	setElement(0,0, d.z * invsqrt);
			setElement(1,0, -d.x*d.y * invsqrt);
				setElement(2,0, d.x);

	setElement(0,1, 0.0);
			setElement(1,1, sq);
				setElement(2,1, d.y);

	setElement(0,2, -d.x * invsqrt);
			setElement(1,2, -d.z*d.y * invsqrt);
				setElement(2,2, d.z);

}

void CMatrix::reset()
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            if (i==j)
                setElement(i,j,1.0);
            else
                setElement(i, j, 0.0);
        }
    }
}

void CMatrix::setCrossProduct(CVector v)
{
	setElement(0,0,0.0); setElement(0,1,-v.z); setElement(0,2,v.y);
	setElement(1,0,v.z); setElement(1,1,0.0); setElement(1,2,-v.x);
	setElement(2,0,-v.y); setElement(2,1,v.x); setElement(2,2,0.0);
}

void CMatrix::setRotation(CVector v)
{
	//the idea is to rotate forward using a matrix A
	//so that v points into the z direction.
	//then rotate around the z axis by the angle |v|
	//then rotate backward using Ainv
	CMatrix A, rot;

	//the collumn vectors of A are v1, v2 and v3
	CVector v1, v2, v3;
	v3 = v.normal();

	if(v3.z < 0.75 && v3.z > -0.75) //not parallel to 0,0,1
		{v2 = CVector(v3.y,-v3.x,0);} //cross product with 0,0,1
	else
		{v2 = CVector(0,v3.z,-v3.y);} //cross product with 1,0,0

	v2.normalise();
	v1 = v3.crossProduct(v2);
	A.setElement(0,0,v1.x);
	A.setElement(1,0,v1.y);
	A.setElement(2,0,v1.z);
	A.setElement(0,1,v2.x);
	A.setElement(1,1,v2.y);
	A.setElement(2,1,v2.z);
	A.setElement(0,2,v3.x);
	A.setElement(1,2,v3.y);
	A.setElement(2,2,v3.z);


	rot.rotZ(v.abs());

	this->operator=(A);
	this->operator*=(rot);
	this->operator/=(A);
}

CMatrix CMatrix::transpose() const
{
	CMatrix ret;

	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			ret.setElement(i,j, Element(j,i));

	return ret;
}
