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

class CGeneralVector : public vector<float>
{
public:
	float norm();
};

class CGeneralMatrix {
public:
	CGeneralMatrix(unsigned int size);
	~CGeneralMatrix();

	enum eSolveMethod
	{
		//Note: only Gauss has currently been implemented
		Gauss, GaussPivot, GaussScaledPivot, LU, Choleski, Crout
	};

	int solve(CGeneralVector &b, eSolveMethod method = Gauss);
	void setElement(unsigned int i, unsigned int j, float value);
	float getElement(unsigned int i, unsigned int j);
	void removeRowCol(unsigned int n);
	bool isIvertible();
	float norm();
protected:
	vector<CGeneralVector> m_Matrix;
	unsigned int m_n;

	int solveGauss(CGeneralVector &b);

	void swaprow(unsigned int i, unsigned int j, CGeneralVector &b);
	void addrow(unsigned int i, float mul, unsigned int j, CGeneralVector &b);
};

#endif
