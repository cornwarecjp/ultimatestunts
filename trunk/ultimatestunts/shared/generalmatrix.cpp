/***************************************************************************
                          generalmatrix.cpp  -  description
                             -------------------
    begin                : do feb 5 2004
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
#include "generalmatrix.h"

CGeneralMatrix::CGeneralMatrix(unsigned int size)
{
	//Create the matrix
	for(unsigned int i=0; i < size; i++)
	{
		CGeneralVector v;
		for(unsigned int j=0; j < size; j++)
			v.push_back(0.0);
		m_Matrix.push_back(v);
	}
	m_n = size;
}

CGeneralMatrix::~CGeneralMatrix()
{
}

int CGeneralMatrix::solve(CGeneralVector &b, eSolveMethod method)
{
	if(b.size() != m_n) return -2; //bad size

	switch(method)
	{
	case Gauss:
		return solveGauss(b);
	default:
		;
	}

	return -1; //succes
}

int CGeneralMatrix::solveGauss(CGeneralVector &b)
{
	for(unsigned int i=0; i < m_n-1; i++)
	{

		unsigned int p = 0;
		while(true)
		{
			if(p == m_n) return i; //no unique solution exists
			float aip = m_Matrix[i][p];
			if(aip < -0.0001 || aip > 0.0001) //!= 0
				break;
			p++;
		}

		if(p != i)
			swaprow(i, p, b);

		float aii = m_Matrix[i][i];
		for(unsigned int j=i+1; j < m_n; j++)
		{
			float mji = m_Matrix[j][i] / aii;
			addrow(j, -mji, i, b);
		}

	}

	float ann = m_Matrix[m_n-1][m_n-1];
	if(ann < 0.0001 && ann > -0.0001) return m_n-1; //no unique solution exists

	b[m_n-1] /= ann;

	if(m_n > 1)
	{
		for(unsigned int i=m_n-2; i >= 0; i--)
		{
			float sum = 0.0;
			for(unsigned int j=i+1; j < m_n; j++)
			{
				sum += m_Matrix[i][j] * b[j];
			}

			b[i] = (b[i] - sum) / m_Matrix[i][i];

			if(i == 0) break; //bugfix for unsigned int behaviour
		}
	}

	return -1; //succes
}

void CGeneralMatrix::swaprow(unsigned int i, unsigned int j, CGeneralVector &b)
{
	for(unsigned int k=0; k < m_n; k++)
	{
		float t = m_Matrix[i][k];
		m_Matrix[i][k] = m_Matrix[j][k];
		m_Matrix[j][k] = t;
	}

	float t = b[i];
	b[i] = b[j];
	b[j] = t;
}

void CGeneralMatrix::addrow(unsigned int i, float mul, unsigned int j, CGeneralVector &b)
{
	for(unsigned int k=0; k < m_n; k++)
		m_Matrix[i][k] += mul * m_Matrix[j][k];

	b[i] += mul * b[j];
}

void CGeneralMatrix::setElement(unsigned int i, unsigned int j, float value)
{
	m_Matrix[i][j] = value;
}

float CGeneralMatrix::getElement(unsigned int i, unsigned int j)
{
	return m_Matrix[i][j];
}

void CGeneralMatrix::removeRowCol(unsigned int n)
{
	for(unsigned int i=0; i < m_n; i++)
		m_Matrix[i].erase(&(m_Matrix[i][n]));

	m_Matrix.erase(&(m_Matrix[n]));

	m_n--;
}

bool CGeneralMatrix::isIvertible()
{
	//NYI
	return true;
}

float CGeneralMatrix::norm()
{
	float norm = 0.0;

	for(unsigned int i=0; i < m_n; i++)
	{
		float sum = 0.0;
		for(unsigned int j=0; j < m_n; j++)
			sum += fabs(m_Matrix[i][j]);

		if(sum > norm) norm = sum;
	}

	return norm;
}

float CGeneralVector::norm()
{
	float norm = 0.0;
	for(unsigned int i=0; i < size(); i++)
	{
		float n = fabs((*this)[i]);
		if(n > norm) norm = n;
	}
	return norm;
	
}
