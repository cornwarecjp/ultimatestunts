/***************************************************************************
                          collisionface.cpp  -  description
                             -------------------
    begin                : wo nov 26 2003
    copyright            : (C) 2003 by CJP
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

#include "collisionface.h"

CCollisionFace::CCollisionFace()
{
	material = NULL;
}

CCollisionFace::CCollisionFace(const CCollisionFace &f)
{
	for(unsigned int i=0; i < f.size(); i++)
		push_back(f[i]);

	d = f.d;
	nor = f.nor;
	reverse = f.reverse;
	material = f.material;

	isWater = f.isWater;
	isSurface = f.isSurface;
}

const CCollisionFace &CCollisionFace::operator=(const CCollisionFace &f)
{
	clear();
	for(unsigned int i=0; i < f.size(); i++)
		push_back(f[i]);

	d = f.d;
	nor = f.nor;
	reverse = f.reverse;
	material = f.material;

	isWater = f.isWater;
	isSurface = f.isSurface;

	return *this;
}

void CCollisionFace::cull(const CCollisionFace &plane, const CVector &dr)
{
	//translation of the plane
	CVector plane_nor = plane.nor;
	float plane_d = plane.d + dr.dotProduct(plane.nor);

	CCollisionFace theFace2;
	theFace2.nor = nor;
	theFace2.d = d;
	theFace2.material = material;
	theFace2.isSurface = isSurface;
	theFace2.isWater = isWater;

	//The first vertex
	float dist_first = operator[](0).dotProduct(plane_nor);
	bool inside_first = (dist_first < plane_d);
	if(inside_first)
	{
		theFace2.push_back(operator[](0));
	}
	float dist_prev = dist_first;
	bool inside_prev = inside_first;

	//The next vertices
	for(unsigned int j=1; j < size(); j++)
	{
		float dist = operator[](j).dotProduct(plane_nor);
		bool inside = (dist < plane_d);

		if(inside)
		{
			if(!inside_prev) //going inward
			{
				CVector mid = ((dist_prev-plane_d)*operator[](j) + (plane_d-dist)*operator[](j-1)) / (dist_prev-dist);
				theFace2.push_back(mid);
			}

			theFace2.push_back(operator[](j));
		}
		else if(inside_prev) //going outward
		{
			CVector mid = ((dist_prev-plane_d)*operator[](j) + (plane_d-dist)*operator[](j-1)) / (dist_prev-dist);
			theFace2.push_back(mid);
		}

		inside_prev = inside;
		dist_prev = dist;
	}

	//The last vertex
	if(inside_prev != inside_first) //first+last not the same
	{
		unsigned int j = size()-1;
		CVector mid = ((dist_prev-plane_d)*operator[](0) + (plane_d-dist_first)*operator[](j)) / (dist_prev-dist_first);
		theFace2.push_back(mid);
	}

	operator=(theFace2);
}

const CCollisionFace &CCollisionFace::operator+=(const CVector &r)
{
	for(unsigned int i=0; i < size(); i++)
		operator[](i) += r;

	d += r.dotProduct(nor);

	return *this;
}

const CCollisionFace &CCollisionFace::operator*=(const CMatrix &m)
{
	for(unsigned int i=0; i < size(); i++)
		operator[](i) *= m;

	nor *= m;

	return *this;
}

const CCollisionFace &CCollisionFace::operator/=(const CMatrix &m)
{
	for(unsigned int i=0; i < size(); i++)
		operator[](i) /= m;

	nor /= m;

	return *this;
}
