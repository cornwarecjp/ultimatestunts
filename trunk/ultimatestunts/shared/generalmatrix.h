/***************************************************************************
                          generalmatrix.h  -  description
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

#ifndef GENERALMATRIX_H
#define GENERALMATRIX_H


/**
  *@author CJP
  */

#include <vector>
namespace std {}
using namespace std;

#define GM_GAUSS	1
#define GM_PIVOT	2
#define GM_SCALED	4
#define GM_MODIFIED	8

#define GM_LU		16
#define GM_CHOLESKI	32
#define GM_CROUT	64

class CGeneralVector : public vector<float>
{
public:
	float norm();
};

class CGeneralMatrix {
public:
	CGeneralMatrix(unsigned int size);
	~CGeneralMatrix();

	int solve(CGeneralVector &b, unsigned int solvemethod=GM_GAUSS);
	void setElement(unsigned int i, unsigned int j, float value);
	float getElement(unsigned int i, unsigned int j);
	void removeRowCol(unsigned int n);
	bool isIvertible();
	float norm();

	bool m_debug;
protected:
	vector<CGeneralVector> m_Matrix;
	unsigned int m_numRows, m_numCols;

	int solveGauss(CGeneralVector &b);
	int solveModified(CGeneralVector &b);

	void swaprow(unsigned int i, unsigned int j, CGeneralVector &b);
	void addrow(unsigned int i, float mul, unsigned int j, CGeneralVector &b);

	bool isSmall(float val);
};

#endif
