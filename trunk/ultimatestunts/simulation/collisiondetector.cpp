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
#include "collision.h"
#include "world.h"

CCollisionDetector::CCollisionDetector()
{
	m_FirstUpdate = true;
}

CCollisionDetector::~CCollisionDetector()
{
}

void CCollisionDetector::calculateCollisions()
{
	if(m_FirstUpdate)
	{
		m_FirstUpdate = false;
		calculateTrackBounds();
	}

	//Collision with track bounds
	for(unsigned int i=0; i < theWorld->m_MovObjs.size(); i++)
		for(unsigned int j=0; j < theWorld->m_MovObjs[i]->m_Bodies.size(); j++)
			ObjTrackBoundTest(theWorld->m_MovObjs[i]->m_Bodies[j]);

	//Moving object to moving object collisions
	for(unsigned int i1=0; i1 < theWorld->m_MovObjs.size()-1; i1++)
		for(unsigned int i2=i1+1; i2 < theWorld->m_MovObjs.size(); i2++)
			for(unsigned int j1=0; j1 < theWorld->m_MovObjs[i1]->m_Bodies.size(); j1++)
				for(unsigned int j2=0; j2 < theWorld->m_MovObjs[i2]->m_Bodies.size(); j2++)
					ObjObjTest(theWorld->m_MovObjs[i1]->m_Bodies[j1], theWorld->m_MovObjs[i2]->m_Bodies[j2]);

	//Moving object to tile collisions
	int lengte = theWorld->m_L;
	int breedte = theWorld->m_W;
	int hoogte = theWorld->m_H;
	for(unsigned int i=0; i < theWorld->m_MovObjs.size(); i++)
	{
		CVector pos = theWorld->m_MovObjs[i]->m_Bodies[0].getPosition();

		int x = (int)(0.5 + (pos.x)/TILESIZE);
		int z = (int)(0.5 + (pos.z)/TILESIZE);

		int xmin = (x-1 < 0)? 0 : x-1;
		int xmax = (x+1 > lengte-1)? lengte-1 : x+1;
		int zmin = (z-1 < 0)? 0 : z-1;
		int zmax = (z+1 > breedte-1)? breedte-1 : z+1;

		for(x=xmin; x<=xmax; x++)
			for(z=zmin; z<=zmax; z++)
				for(int h=0; h<hoogte; h++)
					ObjTileTest(i, x, z, h);
	}
}

void CCollisionDetector::calculateTrackBounds()
{
	int minz = 0;
	int maxz = 0;

	int lth = theWorld->m_L;
	int wth = theWorld->m_W;
	int hth = theWorld->m_H;
	for(int x = 0; x < lth; x++)
		for(int y = 0; y < wth; y++)
			for(int h = 0; h < hth; h++)
			{
				int z = theWorld->m_Track[h + hth*(y + wth*x)].m_Z;
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

void CCollisionDetector::ObjTrackBoundTest(const CBody &body)
{
	CVector p = body.getPosition();
	CVector v = body.getVelocity();

	const CBound *b = theWorld->m_MovObjBounds[body.m_Body];
	float r = b->m_BSphere_r;

	if(p.x - r < m_TrackMin.x)
	{
		dContact c;
		c.geom.pos[0] = m_TrackMin.x;
		c.geom.pos[1] = p.y;
		c.geom.pos[2] = p.z;
		c.geom.normal[0] = 1;
		c.geom.normal[1] = 0;
		c.geom.normal[2] = 0;
		c.geom.depth = m_TrackMin.x + r - p.x;
		c.geom.g1 = 0; //I don't use geoms here
		c.geom.g2 = 0;
		c.surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1;
		c.surface.mu = 0;
		c.surface.slip1 = 0.00001;
		c.surface.slip2 = 0.00001;
		dJointID cid = dJointCreateContact(theWorld->m_ODEWorld, theWorld->m_ContactGroup, &c);
		dJointAttach(cid, body.m_ODEBody, 0);
	}
	if(p.y - r < m_TrackMin.y) //the bottom of the track
	{
		dContact c;
		c.geom.pos[0] = p.x;
		c.geom.pos[1] = m_TrackMin.y;
		c.geom.pos[2] = p.z;
		c.geom.normal[0] = 0;
		c.geom.normal[1] = 1;
		c.geom.normal[2] = 0;
		c.geom.depth = m_TrackMin.y + r - p.y + 1.0; //give it some vertical speed
		c.geom.g1 = 0; //I don't use geoms here
		c.geom.g2 = 0;
		c.surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1;
		c.surface.mu = 0;
		c.surface.slip1 = 0.00001;
		c.surface.slip2 = 0.00001;
		dJointID cid = dJointCreateContact(theWorld->m_ODEWorld, theWorld->m_ContactGroup, &c);
		dJointAttach(cid, body.m_ODEBody, 0);
	}
	if(p.z - r < m_TrackMin.z)
	{
		dContact c;
		c.geom.pos[0] = p.x;
		c.geom.pos[1] = p.y;
		c.geom.pos[2] = m_TrackMin.z;
		c.geom.normal[0] = 0;
		c.geom.normal[1] = 0;
		c.geom.normal[2] = 1;
		c.geom.depth = m_TrackMin.z + r - p.z;
		c.geom.g1 = 0; //I don't use geoms here
		c.geom.g2 = 0;
		c.surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1;
		c.surface.mu = 0;
		c.surface.slip1 = 0.00001;
		c.surface.slip2 = 0.00001;
		dJointID cid = dJointCreateContact(theWorld->m_ODEWorld, theWorld->m_ContactGroup, &c);
		dJointAttach(cid, body.m_ODEBody, 0);
	}
	if(p.x + r > m_TrackMax.x)
	{
		dContact c;
		c.geom.pos[0] = m_TrackMax.x;
		c.geom.pos[1] = p.y;
		c.geom.pos[2] = p.z;
		c.geom.normal[0] = -1;
		c.geom.normal[1] = 0;
		c.geom.normal[2] = 0;
		c.geom.depth = p.x - r - m_TrackMax.x;
		c.geom.g1 = 0; //I don't use geoms here
		c.geom.g2 = 0;
		c.surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1;
		c.surface.mu = 0;
		c.surface.slip1 = 0.00001;
		c.surface.slip2 = 0.00001;
		dJointID cid = dJointCreateContact(theWorld->m_ODEWorld, theWorld->m_ContactGroup, &c);
		dJointAttach(cid, body.m_ODEBody, 0);
	}
	if(p.y + r > m_TrackMax.y)
	{
		dContact c;
		c.geom.pos[0] = p.x;
		c.geom.pos[1] = m_TrackMax.y;
		c.geom.pos[2] = p.z;
		c.geom.normal[0] = 0;
		c.geom.normal[1] = -1;
		c.geom.normal[2] = 0;
		c.geom.depth = p.y - r - m_TrackMax.y;
		c.geom.g1 = 0; //I don't use geoms here
		c.geom.g2 = 0;
		c.surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1;
		c.surface.mu = 0;
		c.surface.slip1 = 0.00001;
		c.surface.slip2 = 0.00001;
		dJointID cid = dJointCreateContact(theWorld->m_ODEWorld, theWorld->m_ContactGroup, &c);
		dJointAttach(cid, body.m_ODEBody, 0);
	}
	if(p.z + r > m_TrackMax.z)
	{
		dContact c;
		c.geom.pos[0] = p.x;
		c.geom.pos[1] = p.y;
		c.geom.pos[2] = m_TrackMax.z;
		c.geom.normal[0] = 0;
		c.geom.normal[1] = 0;
		c.geom.normal[2] = -1;
		c.geom.depth = p.z - r - m_TrackMax.z;
		c.geom.g1 = 0; //I don't use geoms here
		c.geom.g2 = 0;
		c.surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1;
		c.surface.mu = 0;
		c.surface.slip1 = 0.00001;
		c.surface.slip2 = 0.00001;
		dJointID cid = dJointCreateContact(theWorld->m_ODEWorld, theWorld->m_ContactGroup, &c);
		dJointAttach(cid, body.m_ODEBody, 0);
	}
}

#define elasticity 0.1
#define responsefactor (1.0 + elasticity)
#define dynfriction 100

void CCollisionDetector::ObjObjTest(const CBody &body1, const CBody &body2)
{
	CVector p1 = body1.getPosition();
	CVector p2 = body2.getPosition();
	const CBound *b1 = theWorld->m_MovObjBounds[body1.m_Body];
	const CBound *b2 = theWorld->m_MovObjBounds[body2.m_Body];

	if(sphereTest(p1, b1, p2, b2))
	{
		//printf("Collision between %d and %d\n", n1, n2);
		CVector nor = p2 - p1;
		nor.normalise();
		CVector pos = p1 + b1->m_BSphere_r * nor;
		
		dContact c;
		c.geom.pos[0] = pos.x;
		c.geom.pos[1] = pos.y;
		c.geom.pos[2] = pos.z;
		c.geom.normal[0] = nor.x;
		c.geom.normal[1] = nor.y;
		c.geom.normal[2] = nor.z;
		c.geom.depth = -0.5 * (b1->m_BSphere_r + b2->m_BSphere_r - (p2-p1).abs());
		c.geom.g1 = 0; //I don't use geoms here
		c.geom.g2 = 0;
		c.surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1;
		c.surface.mu = 1.0;
		c.surface.slip1 = 0.00001;
		c.surface.slip2 = 0.00001;
		dJointID cid = dJointCreateContact(theWorld->m_ODEWorld, theWorld->m_ContactGroup, &c);
		dJointAttach(cid, body2.m_ODEBody, body1.m_ODEBody);
	}
}

void CCollisionDetector::ObjTileTest(int nobj, int xtile, int ztile, int htile)
{
	//Tile data
	const CTile &theTile = theWorld->m_Track[htile + theWorld->m_H*(ztile + theWorld->m_W*xtile)];
	CVector tilepos = CVector(xtile*TILESIZE, theTile.m_Z*VERTSIZE, ztile*TILESIZE);
	CTileModel *tilemodel = theWorld->m_TileModels[theTile.m_Model];

	//Object data
	CMovingObject *theObj = theWorld->m_MovObjs[nobj];

	//fprintf(stderr, "New tile\n");
	for(unsigned int i=0; i<theObj->m_Bodies.size(); i++)
	{
		//body position & velocity
		CVector pos = theObj->m_Bodies[i].getPosition();
		CMatrix ori = theObj->m_Bodies[i].getOrientationMatrix();
		CVector vel = theObj->m_Bodies[i].getVelocity();

		//relative to the tile
		CVector r = pos - tilepos;

		//position change
		//CVector dr = r1 - r0;

		//the collision model of the body
		const CBound *theBound = theWorld->m_MovObjBounds[theObj->m_Bodies[i].m_Body];

		//fprintf(stderr, "Body %d: r = %s\n", i, CString(r).c_str());

		//Test bounding spheres
		if(!sphereTest(pos, theBound, tilepos, tilemodel)) continue;

		//fprintf(stderr, "Test 2: succesful sphere test\n");

		//Per tile-face
		for(unsigned int tf=0; tf<tilemodel->m_Faces.size(); tf++)
		{
			//fprintf(stderr, "Tileface # %d\n", tf);

			if(tilemodel->m_Faces[tf].nor.abs2() < 0.5) continue; //invalid plane
			//fprintf(stderr, "Test 4: it's a valid plane\n");

			//1: face-sphere collision test

			//1.1: Make a rotated copy
			CCollisionFace theFace = tilemodel->m_Faces[tf];
			for(unsigned int j=0; j<theFace.size(); j++)
				tileRotate(theFace[j], theTile.m_R);

			tileRotate(theFace.nor, theTile.m_R);

			if(theFace.nor.dotProduct(vel) > 0.0)
			{
				//continue; //this contains a bug. But what??
			}

			//1.2: Calculate distance to plane
			float ddiff = r.dotProduct(theFace.nor) - theFace.d;
			float sph_r = theBound->m_BSphere_r;
			//fprintf(stderr, "ddiff = %.3f sph_r = %.3f\n", ddiff, sph_r);

			if(ddiff > sph_r)
			{
				continue;
			}
			//fprintf(stderr, "Test 5: not too far above plane\n");
			if(ddiff < 0.0)
			{
				continue;
			}
			//fprintf(stderr, "Test 6: not below plane\n");

			CVector cpos, cnor;
			float penetr_depth;
			
			if(theBound->m_isCylinder)
			{
				CVector posleft(-0.5*theBound->m_CyliderWidth,0,0);
				CVector posright(0.5*theBound->m_CyliderWidth,0,0);
				posleft *= ori;
				posright *= ori;
				posleft += r; //to tile coordinates
				posright += r;
				float hleft = posleft.dotProduct(theFace.nor) - theFace.d;
				float hright = posright.dotProduct(theFace.nor) - theFace.d;
				float radius = theBound->m_CylinderRadius;
				bool inleft =  hleft >= -radius && hleft <= radius;
				bool inright =  hleft >= -radius && hleft <= radius;

				CVector posmid;
				float h;
				if(!inleft)
				{
					if(!inright) continue;

					//else:
					posmid = posright; h = hright;
				}
				else
				{
					if(!inright)
						{posmid = posleft; h = hleft;}
					else
						{posmid = 0.5*(posleft+posright); h = 0.5*(hleft+hright);}
				}

				cpos = posmid - h * theFace.nor;
				cnor = theFace.nor;
				penetr_depth = radius - h;

				//check if collision  circle is inside the face
				float angle = 0.0;
				for(unsigned int j=1; j<theFace.size(); j++)
				{
					CVector p1 = theFace[j] - cpos;
					CVector p2 = theFace[j-1] - cpos;
					float inpr = p1.dotProduct(p2);
					inpr /= (p1.abs() * p2.abs());
					angle += acos(inpr);
				}
				{
					CVector p1 = theFace[0] - cpos;
					CVector p2 = theFace.back() - cpos;
					float inpr = p1.dotProduct(p2);
					inpr /= (p1.abs() * p2.abs());
					angle += acos(inpr);
				}
				if(angle < 6.0 || angle > 6.5) //!= 2*pi: outside face
				{
					continue;
				}

				cpos += tilepos; //to world coordinates
			}
			else //no cylinder, but a convex volume
			{
				//2: Polygon/bounding faces test:

				//2.1: Rotate + translate the face
				theFace += -r;
				theFace /= ori;

				//2.2: Rotate the vel vector
				vel /= ori;

				//2.3: Cut pieces away per plane
				for(unsigned int of=0; of < theBound->m_Faces.size(); of++)
				{
					//fprintf(stderr, "  %d -> Culling with face %d ->", theFace.size(), of);
					theFace.cull(theBound->m_Faces[of], CVector(0,0,0));
					//fprintf(stderr, "%d\n", theFace.size());

					/*
					if(vel.dotProduct(theBound->m_Faces[of].nor) > 0.0)
					{ //front side -> cull on position r+dr
						theFace.cull(theBound->m_Faces[of], CVector(0,0,0));
					}
					else
					{ //back side -> cull on position r
						theFace.cull(theBound->m_Faces[of], -vel);
					}
					*/

					if(theFace.size() < 3) break;
				}

				//2.4: skip if no piece left
				if(theFace.size() < 3)
					continue;
				//fprintf(stderr, "Test 7: face not clipped away\n");

				//2.5: calculate penetration depth
				penetr_depth = 0.0;
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
				//float max_dist = -(dr.dotProduct(theFace.nor));
				
				cnor = ori * theFace.nor; //rotate back
				cpos = theBound->m_Points[colpt] + penetr_depth * cnor;
				/*
				if(theBound->m_isCylinder) //e.g. a wheel: correct for edges in the collision model
				{
					cpos = -cnor * cpos.abs();
				}
				*/
				cpos += pos; //to absolute coordinates
			}


			//3: Generate collision info

			//fprintf(stderr, "COLLISION body %d @ %s, nor = %s, depth = %.3f\n",
			//	i, CString(cpos - pos).c_str(), CString(cnor).c_str(), penetr_depth);
			dContact c;
			c.geom.pos[0] = cpos.x;
			c.geom.pos[1] = cpos.y;
			c.geom.pos[2] = cpos.z;
			c.geom.normal[0] = cnor.x;
			c.geom.normal[1] = cnor.y;
			c.geom.normal[2] = cnor.z;
			c.geom.depth = penetr_depth;
			c.geom.g1 = 0; //I don't use geoms here
			c.geom.g2 = 0;
			c.surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1;
			c.surface.mu = 10.0;
			c.surface.slip1 = 0.0000001;
			c.surface.slip2 = 0.0000001;
			dJointID cid = dJointCreateContact(theWorld->m_ODEWorld, theWorld->m_ContactGroup, &c);
			dJointAttach(cid, theObj->m_Bodies[i].m_ODEBody, 0);
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
