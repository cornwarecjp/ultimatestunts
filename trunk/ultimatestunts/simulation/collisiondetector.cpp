/***************************************************************************
                          collisiondetector.cpp  -  Detects collisions and resting contacts
                             -------------------
    begin                : ma mrt 22 2004
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

#include <cstdio>
#include <cmath>
#include "usmacros.h"
#include "collisiondetector.h"

CCollisionDetector::CCollisionDetector(const CWorld *w)
{
	m_World = w;
	m_FirstUpdate = true;
}

CCollisionDetector::~CCollisionDetector()
{
}

void CCollisionDetector::calculateCollisions(bool resting)
{
	if(m_FirstUpdate)
	{
		m_FirstUpdate = false;
		calculateTrackBounds();
	}

	m_Collisions.clear();

	if(!resting)
	{
		//Collision with track bounds
		for(unsigned int i=0; i<m_World->m_MovObjs.size(); i++)
			ObjTrackBoundTest(i);

		/*
		//Moving object to moving object collisions
		for(unsigned int i=0; i < m_World->m_MovObjs.size()-1; i++)
			for(unsigned int j=i+1; j < m_World->m_MovObjs.size(); j++)
				ObjObjTest(i,j);
		*/
	}

	//Moving object to tile collisions
	int lengte = m_World->m_L;
	int breedte = m_World->m_W;
	int hoogte = m_World->m_H;
	for(unsigned int i=0; i < m_World->m_MovObjs.size(); i++)
	{
		CVector pos = m_World->m_MovObjs[i]->getPosition();

		int x = (int)(0.5 + (pos.x)/TILESIZE);
		int z = (int)(0.5 + (pos.z)/TILESIZE);

		int xmin = (x-1 < 0)? 0 : x-1;
		int xmax = (x+1 > lengte-1)? lengte-1 : x+1;
		int zmin = (z-1 < 0)? 0 : z-1;
		int zmax = (z+1 > breedte-1)? breedte-1 : z+1;

		for(x=xmin; x<=xmax; x++)
			for(z=zmin; z<=zmax; z++)
				for(int h=0; h<hoogte; h++)
					if(resting)
						{ObjTileTest_resting(i, x, z, h);}
					else
						{ObjTileTest_collision(i, x, z, h);}
	}
}

void CCollisionDetector::calculateTrackBounds()
{
	int minz = 0;
	int maxz = 0;

	int lth = m_World->m_L;
	int wth = m_World->m_W;
	int hth = m_World->m_H;
	for(int x = 0; x < lth; x++)
		for(int y = 0; y < wth; y++)
			for(int h = 0; h < hth; h++)
			{
				int z = m_World->m_Track[h + hth*(y + wth*x)].m_Z;
				if(z < minz) minz = z;
				if(z > maxz) maxz = z;
			}

	m_TrackMin = CVector(
		-0.5*TILESIZE,
		(minz-1)*VERTSIZE,
		-0.5*TILESIZE);
	m_TrackMax = CVector(
		((float)lth-0.5)*TILESIZE,
		(maxz+30)*VERTSIZE,
		((float)wth-0.5)*TILESIZE);
}

void CCollisionDetector::ObjTrackBoundTest(int n)
{
	const CMovingObject *o = m_World->m_MovObjs[n];
	CVector pos0 = o->getPreviousPosition();
	CVector pos1 = o->getPosition();
	CVector mom = o->getMomentum(); //an approximation for the momentum at collision time

	for(unsigned int i=0; i<o->m_Bodies.size(); i++)
	{
		//TODO: time-dependent body position
		CVector p0 = pos0 + o->m_Bodies[i].m_Position;
		CVector p1 = pos1 + o->m_Bodies[i].m_Position;
		const CBound *b = m_World->m_MovObjBounds[o->m_Bodies[i].m_Body];
		float r = b->m_BSphere_r;

		if(p1.x - r < m_TrackMin.x)
		{
			CCollision c;

			c.object1 = -1;
			c.object2 = n;
			c.body1 = -1;
			c.body2 = i;
			c.mat1 = c.mat2 = 0;

			c.p = -1.1*mom.x;

			float d1 = p1.x - r - m_TrackMin.x;
			float d0 = p0.x - r - m_TrackMin.x;
			c.t = d0 / (d1 - d0);
			c.isResting = false;

			CVector pc = c.t * p0 + (1-c.t) * p1;
			c.nor = CVector(1,0,0);
			c.pos = CVector(m_TrackMin.x, pc.y, pc.z);

			m_Collisions.push_back(c);
		}
		if(p1.y - r < m_TrackMin.y) //fallen through the track
		{
			CCollision c;

			c.object1 = -1;
			c.object2 = n;
			c.body1 = -1;
			c.body2 = i;
			c.mat1 = c.mat2 = 0;

			c.p = -1.1*mom.y;

			float d1 = p1.y - r - m_TrackMin.y;
			float d0 = p0.y - r - m_TrackMin.y;
			c.t = d0 / (d1 - d0);
			c.isResting = false;

			CVector pc = c.t * p0 + (1-c.t) * p1;
			c.nor = CVector(0,1,0);
			c.pos = CVector(pc.x, m_TrackMin.y, pc.z);

			m_Collisions.push_back(c);
		}
		if(p1.z - r < m_TrackMin.z)
		{
			CCollision c;

			c.object1 = -1;
			c.object2 = n;
			c.body1 = -1;
			c.body2 = i;
			c.mat1 = c.mat2 = 0;

			c.p = -1.1*mom.z;

			float d1 = p1.z - r - m_TrackMin.z;
			float d0 = p0.z - r - m_TrackMin.z;
			c.t = d0 / (d1 - d0);
			c.isResting = false;

			CVector pc = c.t * p0 + (1-c.t) * p1;
			c.nor = CVector(0,0,1);
			c.pos = CVector(pc.x, pc.y, m_TrackMin.z);

			m_Collisions.push_back(c);
		}
		if(p1.x + r > m_TrackMax.x)
		{
			CCollision c;

			c.object1 = -1;
			c.object2 = n;
			c.body1 = -1;
			c.body2 = i;
			c.mat1 = c.mat2 = 0;

			c.p = -1.1*mom.x;

			float d1 = p1.x + r - m_TrackMax.x;
			float d0 = p0.x + r - m_TrackMax.x;
			c.t = d0 / (d1 - d0);
			c.isResting = false;

			CVector pc = c.t * p0 + (1-c.t) * p1;
			c.nor = CVector(-1,0,0);
			c.pos = CVector(m_TrackMax.x, pc.y, pc.z);

			m_Collisions.push_back(c);
		}
		if(p1.y + r > m_TrackMax.y)
		{
			CCollision c;

			c.object1 = -1;
			c.object2 = n;
			c.body1 = -1;
			c.body2 = i;
			c.mat1 = c.mat2 = 0;

			c.p = -1.1*mom.y;

			float d1 = p1.y + r - m_TrackMax.y;
			float d0 = p0.y + r - m_TrackMax.y;
			c.t = d0 / (d1 - d0);
			c.isResting = false;

			CVector pc = c.t * p0 + (1-c.t) * p1;
			c.nor = CVector(0,-1,0);
			c.pos = CVector(pc.x, m_TrackMax.y, pc.z);

			m_Collisions.push_back(c);
		}
		if(p1.z + r > m_TrackMax.z)
		{
			CCollision c;

			c.object1 = -1;
			c.object2 = n;
			c.body1 = -1;
			c.body2 = i;
			c.mat1 = c.mat2 = 0;

			c.p = -1.1*mom.z;

			float d1 = p1.z + r - m_TrackMax.z;
			float d0 = p0.z + r - m_TrackMax.z;
			c.t = d0 / (d1 - d0);
			c.isResting = false;

			CVector pc = c.t * p0 + (1-c.t) * p1;
			c.nor = CVector(0,0,-1);
			c.pos = CVector(pc.x, pc.y, m_TrackMax.z);

			m_Collisions.push_back(c);
		}
	}
}

#define elasticity 0.1
#define responsefactor (1.0 + elasticity)
#define dynfriction 100

void CCollisionDetector::ObjObjTest(int n1, int n2)
{
	CMovingObject *o1 = m_World->m_MovObjs[n1];
	CMovingObject *o2 = m_World->m_MovObjs[n2];
	CVector pos1 = o1->getPosition();
	CVector pos2 = o2->getPosition();

	//TODO: per body collision
	for(unsigned int i=0; i<o1->m_Bodies.size(); i++)
		for(unsigned int j=0; j<o2->m_Bodies.size(); j++)
		{
			CVector p1 = pos1 + o1->m_Bodies[i].m_Position;
			CVector p2 = pos2 + o2->m_Bodies[j].m_Position;
			const CBound *b1 = m_World->m_MovObjBounds[o1->m_Bodies[i].m_Body];
			const CBound *b2 = m_World->m_MovObjBounds[o2->m_Bodies[i].m_Body];
			if(sphereTest(p1, b1, p2, b2))
			{
				//printf("Collision between %d and %d\n", n1, n2);
				CCollision c;

				c.nor = p2 - p1;
				c.nor.normalise();

				c.pos = p1 + b1->m_BSphere_r * c.nor;

				c.mat1 = c.mat2 = 0;

				c.object1 = n1;
				c.object2 = n2;
				c.body1 = i; c.body2 = j;

				//position correction
				//float dr = -0.5 * (b1->m_BSphere_r + b2->m_BSphere_r - (p2-p1).abs());

				//determine momentum transfer
				{
					float p1 = o1->getMomentum().dotProduct(c.nor);
					float p2 = o2->getMomentum().dotProduct(c.nor);
					float m1 = 1.0 / o1->getInvMass();
					float m2 = 1.0 / o2->getInvMass();
					float vcg = (p1 + p2)/(m1 + m2);
					c.p = responsefactor*m1*(vcg - p1);
				}

				m_Collisions.push_back(c);
			}
		}
}

void CCollisionDetector::ObjTileTest_collision(int nobj, int xtile, int ztile, int htile)
{
	//Tile data
	const CTile &theTile = m_World->m_Track[htile + m_World->m_H*(ztile + m_World->m_W*xtile)];
	CVector tilepos = CVector(xtile*TILESIZE, theTile.m_Z*VERTSIZE, ztile*TILESIZE);
	CTileModel *tilemodel = m_World->m_TileModels[theTile.m_Model];

	//Object data
	CMovingObject *theObj = m_World->m_MovObjs[nobj];
	CVector objpos0 = theObj->getPreviousPosition();
	CVector objpos1 = theObj->getPosition();
	const CMatrix &objori = theObj->getOrientationMatrix();

	for(unsigned int i=0; i<theObj->m_Bodies.size(); i++)
	{
		//body position
		CVector pos0 = objpos0 + theObj->m_Bodies[i].m_PreviousPosition;
		CVector pos1 = objpos1 + theObj->m_Bodies[i].m_Position;

		//relative to the tile
		CVector r0 = pos0 - tilepos;
		CVector r1 = pos1 - tilepos;

		//position change
		CVector dr = r1 - r0;

		//the collision model of the body
		const CBound *theBound = m_World->m_MovObjBounds[theObj->m_Bodies[i].m_Body];

		//Test bounding spheres
		if(!sphereTest(pos1, theBound, tilepos, tilemodel)) continue;

		//Per tile-face
		for(unsigned int tf=0; tf<tilemodel->m_Faces.size(); tf++)
		{
			if(tilemodel->m_Faces[tf].nor.abs2() < 0.5) continue; //invalid plane

			//1: face-sphere collision test

			//1.1: Make a rotated copy
			CCollisionFace theFace = tilemodel->m_Faces[tf];
			for(unsigned int j=0; j<theFace.size(); j++)
				tileRotate(theFace[j], theTile.m_R);

			tileRotate(theFace.nor, theTile.m_R);

			if(theFace.nor.dotProduct(dr) > 0.0)
			{
				//continue; //this contains a bug. But what??
			}

			//1.2: Calculate distance to plane
			float ddiff0 = r0.dotProduct(theFace.nor) - theFace.d;
			float ddiff1 = r1.dotProduct(theFace.nor) - theFace.d;
			float sph_r = theBound->m_BSphere_r;

			if(ddiff1 > sph_r)
			{
				continue;
			}
			if(ddiff1 < 0.0 && ddiff0 < 0.0)
			{
				continue;
			}

			//1.3: check if collision  circle is inside the face
			//This code contains a bug:
			//  currently only the midpoint of the circle is checked
			//  with the polygon, while this should actually
			//  be a full circle/polygon test.
			/*
			CVector curcoll_pos = rplusdr - ddiff * theFace.nor;
			CVector prevcoll_pos = sph_r - prevddiff * theFace.nor;
			CVector coll_pos = ((prevddiff-sph_r)*curcoll_pos + (sph_r-ddiff)*prevcoll_pos) / (prevddiff-ddiff);
			float angle = 0.0;
			for(unsigned int j=1; j<theFace.size(); j++)
			{
				CVector p1 = theFace[j]-coll_pos;
				CVector p2 = theFace[j-1]-coll_pos;
				float inpr = p1.dotProduct(p2);
				inpr /= (p1.abs() * p2.abs());
				angle += acos(inpr);
			}
			{
				CVector p1 = theFace[0]-coll_pos;
				CVector p2 = theFace.back()-coll_pos;
				float inpr = p1.dotProduct(p2);
				inpr /= (p1.abs() * p2.abs());
				angle += acos(inpr);
			}
			if(angle < 6.0 || angle > 6.5) //!= 2*pi: outside face
			{
				//continue;
			}
			*/

			//2: Polygon/bounding faces test:

			//2.1: Rotate + translate the face
			theFace += -r1;
			theFace /= objori;

			//2.2: Rotate the dr vector
			dr /= objori;

			//2.3: Cut pieces away per plane
			for(unsigned int of=0; of < theBound->m_Faces.size(); of++)
			{
				if(dr.dotProduct(theBound->m_Faces[of].nor) > 0.0)
				{ //front side -> cull on position r+dr
					theFace.cull(theBound->m_Faces[of], CVector(0,0,0));
				}
				else
				{ //back side -> cull on position r
					theFace.cull(theBound->m_Faces[of], -dr);
				}

				if(theFace.size() < 3) break;
			}

			//2.4: skip if no piece left
			if(theFace.size() < 3)
				continue;

			//2.5: calculate penetration depth
			float penetr_depth = 0.0;
			unsigned int colpt = 0;
			for(unsigned int pt=0; pt < theBound->m_Points.size(); pt++)
			{
				float depth = theFace.d - theBound->m_Points[pt].dotProduct(theFace.nor);
				if(depth > penetr_depth)
				{
					penetr_depth = depth;
					colpt = pt;
				}
			}
			float max_dist = -(dr.dotProduct(theFace.nor));

			//3: Generate collision info
			CCollision c;

			c.t = 1.0 - penetr_depth / max_dist;
			c.isResting = false;

			//normal+position
			c.nor = objori * theFace.nor; //rotate back
			c.pos = (1-c.t)*pos0 + c.t*pos1 + theBound->m_Points[colpt];

			//Objects+materials
			c.mat1 = c.mat2 = 0;
			c.object1 = -1;
			c.object2 = nobj;
			c.body1 = -1;
			c.body2 = i;

			//determine momentum transfer
			{
				CMatrix R1; R1.setCrossProduct(theBound->m_Points[colpt]);
				float v_eff = c.nor.dotProduct(-(theObj->getVelocity()) + R1 * theObj->getAngularVelocity());

				if(v_eff < 0.0)
					continue; //already moving away from each other

				const CMatrix &Iinv = theObj->getActualInvMomentInertia();
				float minv = theObj->getInvMass();
				float minv_eff = minv + c.nor.dotProduct((R1*Iinv*R1) * c.nor);
				c.p = responsefactor * v_eff / minv_eff;

				c.p *= (1.0 + (R1*(Iinv*(R1*(c.nor*(1.0/minv))))).dotProduct(c.nor));

				if(m_World->printDebug)
				{
					fprintf(stderr, "Added collision: pos = %.3f,%.3f,%.3f; nor = %.3f,%.3f,%.3f; v_eff = %.3f\n",
						c.pos.x, c.pos.y, c.pos.z, c.nor.x, c.nor.y, c.nor.z, v_eff);
				}

			}

			m_Collisions.push_back(c);

		} //for tf
	} //for i
}

void CCollisionDetector::ObjTileTest_resting(int nobj, int xtile, int ztile, int htile)
{
	//Tile data
	const CTile &theTile = m_World->m_Track[htile + m_World->m_H*(ztile + m_World->m_W*xtile)];
	CVector tilepos = CVector(xtile*TILESIZE, theTile.m_Z*VERTSIZE, ztile*TILESIZE);
	CTileModel *tilemodel = m_World->m_TileModels[theTile.m_Model];

	//Object data
	CMovingObject *theObj = m_World->m_MovObjs[nobj];
	CVector objpos1 = theObj->getPosition();
	const CMatrix &objori = theObj->getOrientationMatrix();
	CVector p = theObj->getMomentum();

	for(unsigned int i=0; i<theObj->m_Bodies.size(); i++)
	{
		//body position
		CVector pos1 = objpos1 + theObj->m_Bodies[i].m_Position;

		//relative to the tile
		CVector r1 = pos1 - tilepos;

		//the collision model of the body
		const CBound *theBound = m_World->m_MovObjBounds[theObj->m_Bodies[i].m_Body];

		//Test bounding spheres
		if(!sphereTest(pos1, theBound, tilepos, tilemodel)) continue;

		//Per tile-face
		for(unsigned int tf=0; tf<tilemodel->m_Faces.size(); tf++)
		{
			if(tilemodel->m_Faces[tf].nor.abs2() < 0.5) continue; //invalid plane

			//1: face-sphere collision test

			//1.1: Make a rotated copy
			CCollisionFace theFace = tilemodel->m_Faces[tf];
			for(unsigned int j=0; j<theFace.size(); j++)
				tileRotate(theFace[j], theTile.m_R);

			tileRotate(theFace.nor, theTile.m_R);

			if(theFace.nor.dotProduct(p) > 0.0)
			{
				//continue; //this contains a bug. But what??
			}

			//1.2: Calculate distance to plane
			//float ddiff0 = r0.dotProduct(theFace.nor) - theFace.d;
			float ddiff1 = r1.dotProduct(theFace.nor) - theFace.d;
			float sph_r = theBound->m_BSphere_r;

			if(ddiff1 > sph_r || ddiff1 < 0.0)
			{
				continue;
			}

			//2: Polygon/bounding faces test:

			//2.1: Rotate + translate the face
			theFace += -r1;
			theFace /= objori;

			//2.2: Rotate the p vector
			p /= objori;

			//2.3: Cut pieces away per plane
			for(unsigned int of=0; of < theBound->m_Faces.size(); of++)
			{
				theFace.cull(theBound->m_Faces[of], CVector(0,0,0));
				if(theFace.size() < 3) break;
			}

			//2.4: skip if no piece left
			if(theFace.size() < 3)
				continue;

			//2.5: calculate penetration depth
			float penetr_depth = 0.0;
			for(unsigned int pt=0; pt < theBound->m_Points.size(); pt++)
			{
				float depth = theFace.d - theBound->m_Points[pt].dotProduct(theFace.nor);
				if(depth > penetr_depth)
					penetr_depth = depth;
			}

			//3: Generate collision info
			for(unsigned int j=0; j < theFace.size(); j++)
			{
				CCollision c;

				c.isResting = true;

				//normal+position
				CVector relpos = objori * theFace[j]; //rotate back
				c.nor = objori * theFace.nor; //rotate back
				c.pos = pos1 + relpos;

				//Objects+materials
				c.mat1 = c.mat2 = 0;
				c.object1 = -1;
				c.object2 = nobj;
				c.body1 = -1;
				c.body2 = i;

				
				float v_eff = c.nor.dotProduct(-(theObj->getVelocity()) + relpos.crossProduct(theObj->getAngularVelocity()));
				if(v_eff < 0.0) continue;

				m_Collisions.push_back(c);
			}

		} //for tf
	} //for i
}

void CCollisionDetector::tileRotate(CVector &v, int rot)
{
	switch(rot % 4)
	{
	case 0:
		return;
	case 1:
		v = CVector(v.z, v.y, -v.x);
		return;
	case 2:
		v = CVector(-v.x, v.y, -v.z);
		return;
	case 3:
		v = CVector(-v.z, v.y, v.x);
	}
}

bool CCollisionDetector::sphereTest(const CVector &p1, const CCollisionModel *b1, const CVector &p2, const CCollisionModel *b2)
{
	float abs2 = (p2-p1).abs2();
	float rsum = b1->m_BSphere_r + b2->m_BSphere_r;
	return abs2 < (rsum * rsum);
}
