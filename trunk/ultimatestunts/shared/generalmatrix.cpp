/***************************************************************************
                          generalmatrix.cpp  -  A matrix class to solve matrix equations
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

CGeneralMatrix::CGeneralMatrix(unsigned int rows, unsigned int cols)
{
	//Create the matrix
	m_numRows = rows;
	m_numCols = cols;

	for(unsigned int i=0; i < rows; i++)
	{
		CGeneralVector v;
		for(unsigned int j=0; j < cols; j++)
			v.push_back(0.0);
		m_Matrix.push_back(v);
	}

	m_debug = false;
}

CGeneralMatrix::~CGeneralMatrix()
{
}

int CGeneralMatrix::solve(CGeneralVector *b, unsigned int solvemethod)
{
	if(b->size() != m_numRows) return -2; //bad size

	m_Vector = b;

	switch(solvemethod)
	{
	case GM_GAUSS:
		return solveGauss();
	case (GM_GAUSS | GM_MODIFIED):
		return solveModified();
	case (GM_INEQUAL):
		return solveInequal();
	default:
		;
	}

	return -1; //succes
}

int CGeneralMatrix::solveGauss()
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
			swaprow(i, p);

		float aii = m_Matrix[i][i];
		for(unsigned int j=i+1; j < m_numRows; j++)
		{
			float mji = m_Matrix[j][i] / aii;
			addrow(j, -mji, i);
		}

	}

	float ann = m_Matrix[m_numRows-1][m_numCols-1];
	if(isSmall(ann)) return m_numRows-1; //no unique solution exists

	(*m_Vector)[m_numRows-1] /= ann;

	if(m_numRows > 1)
	{
		//Backward
		for(unsigned int i=m_numRows-2; i >= 0; i--)
		{
			float sum = 0.0;
			for(unsigned int j=i+1; j < m_numCols; j++)
			{
				sum += m_Matrix[i][j] * (*m_Vector)[j];
			}

			(*m_Vector)[i] = ((*m_Vector)[i] - sum) / m_Matrix[i][i];

			if(i == 0) break; //bugfix for unsigned int behaviour
		}
	}

	return -1; //succes
}

int CGeneralMatrix::solveModified()
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

			fprintf(stderr, "   % .3f\n", (*m_Vector)[i]);
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
				if(row != i) swaprow(i, row);
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
			addrow(ii, -miip, i);
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

			fprintf(stderr, "   % .3f\n", (*m_Vector)[i]);
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
			if((*m_Vector)[i] < 0.0)
			{
				(*m_Vector)[i]=-(*m_Vector)[i];
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
					{m_Vector->insert(m_Vector->begin()+ii, (*m_Vector)[i] * pval / psum);}
				else
					{m_Vector->insert(m_Vector->begin()+ii, 0.0);}

			}

			//Remove stuff in row i
			for(unsigned int j=p+1; j < p+nump; j++)
				m_Matrix[i][j] = 0.0;
			if(p+nump < m_numCols)
				if(p != psmall)
					for(unsigned int j=p+nump; j < m_numCols; j++)
						m_Matrix[i][j] = 0.0;
			if(m_Matrix[i][p] > 0.0)
				{(*m_Vector)[i] = (*m_Vector)[i] * m_Matrix[i][p] / psum;}
			else
				{(*m_Vector)[i] = 0.0;}

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

			fprintf(stderr, "   % .3f\n", (*m_Vector)[i]);
		}
	}

	//Continuing Gauss...
	float ann = m_Matrix[m_numRows-1][m_numCols-1];
	if(isSmall(ann))
	{(*m_Vector)[m_numRows-1] = 0.0;}
	else
	{(*m_Vector)[m_numRows-1] /= ann;}

	

	if(m_numRows > 1)
	{
		//Backward
		for(unsigned int i=m_numRows-2; i >= 0; i--)
		{
			float sum = 0.0;
			for(unsigned int j=i+1; j < m_numCols; j++)
			{
				sum += m_Matrix[i][j] * (*m_Vector)[j];
			}

			float aii = m_Matrix[i][i];

			if(isSmall(aii))
				{(*m_Vector)[i] = 0.0;}
			else
				{(*m_Vector)[i] = ((*m_Vector)[i] - sum) / aii;}

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

			fprintf(stderr, "   % .3f\n", (*m_Vector)[i]);
		}
	}

	return -1; //succes
}

int CGeneralMatrix::solveInequal()
{
	//Find a minumum that satisfies all inequalities

	//Normalise the vectors:
	for(unsigned int i=0; i < m_numRows; i++)
	{
		(*m_Vector)[i] /= m_Matrix[i].abs();
		m_Matrix[i].normalise();
	}

	if(m_debug)
	{
		fprintf(stderr, "Size: %dx%d\n", m_numRows, m_numCols);
		for(unsigned int i=0; i < m_numRows; i++)
		{
			for(unsigned int j=0; j < m_numCols; j++)
				fprintf(stderr, "% .3f  ", m_Matrix[i][j]);

			fprintf(stderr, "   % .3f\n", (*m_Vector)[i]);
		}
	}

	//First order:
	bool found = false;
	float minval = 0.0;
	unsigned int mini = 0;
	for(unsigned int i=0; i < m_numRows; i++)
	{
		CGeneralVector smallest = m_Matrix[i];
		bool restricted = false;
		for(unsigned int j=0; j < m_numCols; j++)
			if(smallest[j] < 0.0)
				{smallest[j] = 0.0; restricted = true;} //restriction
		if(restricted)
		{
			float dotpr = smallest.dotProduct(m_Matrix[i]);
			if(dotpr < 0.01) continue;
			smallest *= ((*m_Vector)[i] / dotpr);
		}
		else
		{
			smallest *= (*m_Vector)[i];
		}
		
		if( satisfiesInequalities(smallest) )
		{
			if(m_debug)
				fprintf(stderr, "Minimum of equation %d satisfies all inequalities\n", i);

			float val = (*m_Vector)[i];
			if(!found || val < minval)
			{
				found = true;
				minval = val;
				mini = i;
			}
		}
	}

	if(found)
	{
		if(m_debug)
			fprintf(stderr, "found mini = %d\n", mini);

		CGeneralVector smallest = m_Matrix[mini];
		bool restricted = false;
		for(unsigned int j=0; j < m_numCols; j++)
			if(smallest[j] < 0.0)
				{smallest[j] = 0.0; restricted = true;} //restriction
		if(restricted)
		{
			float dotpr = smallest.dotProduct(m_Matrix[mini]);
			smallest *= ((*m_Vector)[mini] / dotpr);
		}
		else
		{
			smallest *= (*m_Vector)[mini];
		}

		for(unsigned int j=0; j < m_numRows; j++)
		{
			(*m_Vector)[j] = smallest[j];

			if(m_debug)
				fprintf(stderr, "%.3f\n", (*m_Vector)[j]);
		}
		return -1;
	}

	/*
	if(m_numRows > 2) //doing order=2,...,N-1 solving
	for(unsigned int order=2; order < m_numRows; order++)
	{
		if(order > 2) break;

		if(m_debug)
			fprintf(stderr, "%dth order inequality solving\n", order);

		//search variables
		bool found = false;
		float minval = 0.0;
		CGeneralVector minsol(m_numCols);
		
		//declaration of equation variables
		CGeneralMatrix m2(order, m_numCols);
		CGeneralVector v(order);
		vector<unsigned int> ia;

		//set ia to first combination
		for(unsigned int i=0; i < order; i++)
			ia.push_back(i);

		//try all combinations
		while(true)
		{
			if(m_debug)
				fprintf(stderr, "Trying: ");

			//Fill with current combination
			for(unsigned int i=0; i < order; i++)
			{
				for(unsigned int j=0; j < m_numCols; j++)
					m2.setElement(i, j, m_Matrix[ia[i]][j]);

				v[i] = (*m_Vector)[ia[i]];
				
				if(m_debug)
					fprintf(stderr, "%d ", ia[i]);
			}
			if(m_debug)
				fprintf(stderr, "\n");

			CGeneralMatrix mt = m2.transpose();
			CGeneralMatrix mmt = m2 * mt;

			if(m_debug)
			{
				fprintf(stderr, "mmt =\n");
				fprintf(stderr, "Size: %dx%d\n", mmt.getNumRows(), mmt.getNumCols());
				for(unsigned int i=0; i < mmt.getNumRows(); i++)
				{
					for(unsigned int j=0; j < mmt.getNumCols(); j++)
						fprintf(stderr, "% .3f  ", mmt.getElement(i, j));

					fprintf(stderr, "   % .3f\n", v[i]);
				}
			}

			if(mmt.solve(&v, GM_GAUSS) == -1) //solving worked, solution is in v
			{
				if(m_debug)
					fprintf(stderr, "solving the matrix equation worked\n");

				CGeneralVector solution = mt * v;
				
				if(satisfiesInequalities(solution))
				{
					float val = solution.abs2();
					if(!found || val < minval)
					{
						found = true;
						minval = val;
						minsol = solution;
					}
				}
			}

			if(m_debug)
				fprintf(stderr, "Trying to find the next combination\n");

			//try to find a next combination, or break if we've had everything
			bool changed = false;
			for(int changing=order-1; changing >= 0; changing--)
				if(ia[changing] < changing+m_numRows-order) //we can change it
				{
					if(m_debug)
						fprintf(stderr, "changing ia[%d] to %d\n", changing, ia[changing]+1);

					changed = true;
					ia[changing]++;

					if(changing < (int)order-1)
						for(unsigned int i=changing+1; i < order; i++)
							ia[i] = ia[i-1] + 1;

					break;
				}

			if(!changed) break;
		}

		if(found)
		{
			if(m_debug)
				fprintf(stderr, "Found a %dnd-order solution\n", order);
				
			for(unsigned int j=0; j < m_numRows; j++)
			{
				(*m_Vector)[j] = minsol[j];
			}
			return -1;
		}
	}
	*/

	if(m_debug)
		fprintf(stderr, "Other order solving failed; doing exact solving\n");

	return 0; //failed
	return solveGauss();
}

bool CGeneralMatrix::satisfiesInequalities(const CGeneralVector &m) const
{
	bool isvalid = true;
	for(unsigned int i=0; i < m_numRows; i++)
	{
		float dotpr = 1.1 * m.dotProduct(m_Matrix[i]);
		if(dotpr < (*m_Vector)[i])
		{
			if(m_debug)
				fprintf(stderr, "%.3f < %.3f: does not satisfy inequality %d\n", dotpr, (*m_Vector)[i], i);
			
			isvalid = false; break;
		}
	}

	return isvalid;
}

void CGeneralMatrix::swaprow(unsigned int i, unsigned int j)
{
	for(unsigned int k=0; k < m_numCols; k++)
	{
		float t = m_Matrix[i][k];
		m_Matrix[i][k] = m_Matrix[j][k];
		m_Matrix[j][k] = t;
	}

	float t = (*m_Vector)[i];
	(*m_Vector)[i] = (*m_Vector)[j];
	(*m_Vector)[j] = t;
}

void CGeneralMatrix::addrow(unsigned int i, float mul, unsigned int j)
{
	for(unsigned int k=0; k < m_numCols; k++)
		m_Matrix[i][k] += mul * m_Matrix[j][k];

	(*m_Vector)[i] += mul * (*m_Vector)[j];
}

void CGeneralMatrix::setElement(unsigned int i, unsigned int j, float value)
{
	m_Matrix[i][j] = value;
}

float CGeneralMatrix::getElement(unsigned int i, unsigned int j) const
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

CGeneralVector CGeneralMatrix::operator*(const CGeneralVector &val) const
{
	CGeneralVector ret;

	for(unsigned int i=0; i < m_numRows; i++)
	{
		float sum = 0.0;
		for(unsigned int j=0; j < m_numCols; j++)
			sum += val[j] * m_Matrix[i][j];

		ret.push_back(sum);
	}

	return ret;
}

CGeneralMatrix CGeneralMatrix::operator*(const CGeneralMatrix &val) const
{
	CGeneralMatrix ret(m_numRows, val.getNumCols());

	for(unsigned int i=0; i < m_numRows; i++)
		for(unsigned int j=0; j < val.getNumCols(); j++)
		{
			float sum = 0.0;
			for(unsigned int k=0; k < m_numCols; k++)
				sum += m_Matrix[i][k] * val.getElement(k, j);

			ret.setElement(i, j, sum);
		}
	
	return ret;
}

CGeneralMatrix CGeneralMatrix::transpose()
{
	CGeneralMatrix ret(m_numCols, m_numRows);

	for(unsigned int i=0; i < m_numCols; i++)
		for(unsigned int j=0; j < m_numRows; j++)
			ret.setElement(i, j, m_Matrix[j][i]);

	return ret;
}

bool CGeneralMatrix::isSmall(float val)
{
	return (val > -0.001 && val < 0.001);
}
