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
#include <cstdio>
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
	m_numRows = m_numCols = size;

	m_debug = false;
}

CGeneralMatrix::~CGeneralMatrix()
{
}

int CGeneralMatrix::solve(CGeneralVector &b, unsigned int solvemethod)
{
	if(b.size() != m_numRows) return -2; //bad size

	switch(solvemethod)
	{
	case GM_GAUSS:
		return solveGauss(b);
	case (GM_GAUSS | GM_MODIFIED):
		return solveModified(b);
	default:
		;
	}

	return -1; //succes
}

int CGeneralMatrix::solveGauss(CGeneralVector &b)
{
	//Forward
	for(unsigned int i=0; i < m_numRows-1; i++)
	{

		unsigned int p = 0;
		while(true)
		{
			if(p == m_numCols) return i; //no unique solution exists
			float aip = m_Matrix[i][p];
			if(!isSmall(aip)) //!= 0
				break;
			p++;
		}

		if(p != i)
			swaprow(i, p, b);

		float aii = m_Matrix[i][i];
		for(unsigned int j=i+1; j < m_numRows; j++)
		{
			float mji = m_Matrix[j][i] / aii;
			addrow(j, -mji, i, b);
		}

	}

	float ann = m_Matrix[m_numRows-1][m_numCols-1];
	if(isSmall(ann)) return m_numRows-1; //no unique solution exists

	b[m_numRows-1] /= ann;

	if(m_numRows > 1)
	{
		//Backward
		for(unsigned int i=m_numRows-2; i >= 0; i--)
		{
			float sum = 0.0;
			for(unsigned int j=i+1; j < m_numCols; j++)
			{
				sum += m_Matrix[i][j] * b[j];
			}

			b[i] = (b[i] - sum) / m_Matrix[i][i];

			if(i == 0) break; //bugfix for unsigned int behaviour
		}
	}

	return -1; //succes
}

int CGeneralMatrix::solveModified(CGeneralVector &b)
{
	if(m_debug)
		fprintf(stderr, "Modified Gauss solving...\n");
	//Modified gauss solving for equations like
	//SUM(mij*aj) >= bi

	//Add columns for the inequality differences
	for(unsigned int i=0; i < m_numRows; i++)
		for(unsigned int j=0; j < m_numRows; j++)
		{
			float m = 0.0;
			if(i==j) m = -1.0;
			m_Matrix[i].push_back(m);
		}
	m_numCols += m_numRows;

	if(m_debug)
	{
		fprintf(stderr, "After adding inequality collumns:\n");
		fprintf(stderr, "Size: %dx%d\n", m_numRows, m_numCols);
		for(unsigned int i=0; i < m_numRows; i++)
		{
			for(unsigned int j=0; j < m_numCols; j++)
				fprintf(stderr, "% .3f  ", m_Matrix[i][j]);

			fprintf(stderr, "   % .3f\n", b[i]);
		}
	}

	//Remember pivot points
	vector<unsigned int> ppoints;
	
	//Forward
	unsigned int p = 0;
	for(unsigned int i=0; i < m_numRows; i++)
	{
		//Find a pivot point
		float pval;
		while(true)
		{
			if(p == m_numCols) return i; //no unique solution exists

			//Find the |largest| element in this row
			unsigned int row=i;
			pval = fabs(m_Matrix[i][p]);
			for(unsigned int ii=i; ii<m_numRows; ii++)
			{
				float a = fabs(m_Matrix[ii][p]);
				if(a > pval)
					{pval = a; row = ii;}
			}

			if(!isSmall(pval)) //!= 0
			{
				if(row != i) swaprow(i, row, b);
				break;
			}
			p++;
		}

		pval = m_Matrix[i][p]; //re-discover the sign

		ppoints.push_back(p);

		if(m_debug)
			fprintf(stderr, "Pivot of row %d: %d\n", i, p);

		for(unsigned int ii=i+1; ii < m_numRows; ii++)
		{
			float miip = m_Matrix[ii][p] / pval;
			addrow(ii, -miip, i, b);
		}

	}

	ppoints.push_back(m_numCols);

	if(m_debug)
	{
		fprintf(stderr, "After forward substitution:\n");
		fprintf(stderr, "Size: %dx%d\n", m_numRows, m_numCols);
		for(unsigned int i=0; i < m_numRows; i++)
		{
			for(unsigned int j=0; j < m_numCols; j++)
				fprintf(stderr, "% .3f  ", m_Matrix[i][j]);

			fprintf(stderr, "   % .3f\n", b[i]);
		}
	}


	//Now do the intelligent row adding
	unsigned int rowOffset = 0;
	for(unsigned int i=0; i < m_numRows; i++)
		if(ppoints[i+1-rowOffset]-ppoints[i-rowOffset] > 1)
		{
			unsigned int p = ppoints[i-rowOffset];
			unsigned int nump = ppoints[i+1-rowOffset]-ppoints[i-rowOffset];

			//First set the sign of b to positive
			if(b[i] < 0.0)
			{
				b[i]=-b[i];
				for(unsigned int j=p; j<m_numCols; j++)
					m_Matrix[i][j] = -m_Matrix[i][j];
			}

			//Now summarize the positive pivot points
			float psum = 0.0;
			for(unsigned int j=p; j < p+nump; j++)
				if(m_Matrix[i][j] > 0.0) psum += m_Matrix[i][j];

			//Summarize the other matrix elements
			float msum = 0.0;
			if(p+nump < m_numCols)
				for(unsigned int j=p+nump; j < m_numCols; j++)
					msum += m_Matrix[i][j];
				
			float msgn = copysign(1.0, msum);

			//Get the smallest / largest pivot
			float small = m_Matrix[i][p];
			unsigned int psmall = p;
			for(unsigned int j=p+1; j < p+nump; j++)
				if(msgn*m_Matrix[i][j] < msgn*small)
				{
					small = m_Matrix[i][j];
					psmall = j;
				}

			//Add nump-1 rows
			for(unsigned int ii=i+1; ii < i+nump; ii++)
			{
				unsigned int ppos = ii - i + p;
				float pval = m_Matrix[i][ppos];
				m_Matrix.insert(m_Matrix.begin()+ii, CGeneralVector());
				
				//Add the pivot point
				for(unsigned int j=0; j < ppos; j++)
					m_Matrix[ii].push_back(0.0);
				m_Matrix[ii].push_back(pval);
				if(ppos != p+nump)
					for(unsigned int j=ppos+1; j < p+nump; j++)
						m_Matrix[ii].push_back(0.0);

				//Add the other matrix elements
				if(p+nump < m_numCols)
					if(ppos == psmall)
					{
						for(unsigned int j=p+nump; j < m_numCols; j++)
							m_Matrix[ii].push_back(m_Matrix[i][j]);
					}
					else
					{
						for(unsigned int j=p+nump; j < m_numCols; j++)
							m_Matrix[ii].push_back(0.0);
					}

				//Add the vector element
				if(pval > 0.0)
					{b.insert(b.begin()+ii, b[i] * pval / psum);}
				else
					{b.insert(b.begin()+ii, 0.0);}

			}

			//Remove stuff in row i
			for(unsigned int j=p+1; j < p+nump; j++)
				m_Matrix[i][j] = 0.0;
			if(p+nump < m_numCols)
				if(p != psmall)
					for(unsigned int j=p+nump; j < m_numCols; j++)
						m_Matrix[i][j] = 0.0;
			if(m_Matrix[i][p] > 0.0)
				{b[i] = b[i] * m_Matrix[i][p] / psum;}
			else
				{b[i] = 0.0;}

			if(m_debug)
				fprintf(stderr, "Added %d rows after row %d\n", nump-1, i-rowOffset);

			i += nump-1;
			m_numRows += nump-1;
			rowOffset += nump-1;
		}

	if(m_debug)
	{
		fprintf(stderr, "After intelligent row adding:\n");
		fprintf(stderr, "Size: %dx%d\n", m_numRows, m_numCols);
		for(unsigned int i=0; i < m_numRows; i++)
		{
			for(unsigned int j=0; j < m_numCols; j++)
				fprintf(stderr, "% .3f  ", m_Matrix[i][j]);

			fprintf(stderr, "   % .3f\n", b[i]);
		}
	}

	//Continuing Gauss...
	float ann = m_Matrix[m_numRows-1][m_numCols-1];
	if(isSmall(ann))
	{b[m_numRows-1] = 0.0;}
	else
	{b[m_numRows-1] /= ann;}

	

	if(m_numRows > 1)
	{
		//Backward
		for(unsigned int i=m_numRows-2; i >= 0; i--)
		{
			float sum = 0.0;
			for(unsigned int j=i+1; j < m_numCols; j++)
			{
				sum += m_Matrix[i][j] * b[j];
			}

			float aii = m_Matrix[i][i];

			if(isSmall(aii))
				{b[i] = 0.0;}
			else
				{b[i] = (b[i] - sum) / aii;}

			if(i == 0) break; //bugfix for unsigned int behaviour
		}
	}

	if(m_debug)
	{
		fprintf(stderr, "After backward substitution:\n");
		fprintf(stderr, "Size: %dx%d\n", m_numRows, m_numCols);
		for(unsigned int i=0; i < m_numRows; i++)
		{
			for(unsigned int j=0; j < m_numCols; j++)
				fprintf(stderr, "% .3f  ", m_Matrix[i][j]);

			fprintf(stderr, "   % .3f\n", b[i]);
		}
	}

	return -1; //succes
}

void CGeneralMatrix::swaprow(unsigned int i, unsigned int j, CGeneralVector &b)
{
	for(unsigned int k=0; k < m_numCols; k++)
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
	for(unsigned int k=0; k < m_numCols; k++)
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
	for(unsigned int i=0; i < m_numRows; i++)
		m_Matrix[i].erase(m_Matrix[i].begin() + n);

	m_Matrix.erase(m_Matrix.begin() + n);

	m_numRows--;
	m_numCols--;
}

bool CGeneralMatrix::isIvertible()
{
	//NYI
	return true;
}

float CGeneralMatrix::norm()
{
	float norm = 0.0;

	for(unsigned int i=0; i < m_numRows; i++)
	{
		float sum = 0.0;
		for(unsigned int j=0; j < m_numCols; j++)
			sum += fabs(m_Matrix[i][j]);

		if(sum > norm) norm = sum;
	}

	return norm;
}

bool CGeneralMatrix::isSmall(float val)
{
	return (val > -0.001 && val < 0.001);
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
