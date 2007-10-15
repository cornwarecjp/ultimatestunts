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

void CCollisionDetector::reset()
{
	m_FirstUpdate = true;
}

float CCollisionDetector::getLineCollision(const CVector &pos, const CVector &dir)
{
	CVector dirnorm = dir.normal();

	int lengte = theWorld->getTrack()->m_L;
	int breedte = theWorld->getTrack()->m_W;
	int hoogte = theWorld->getTrack()->m_H;

	int xp = (int)(0.5 + (pos.x)/TILESIZE);
	int zp = (int)(0.5 + (pos.z)/TILESIZE);

	int xmin=xp, xmax = xp;
	int zmin=zp, zmax = zp;

	if(dir.x < 0.0) {xmin-=2;} else {xmax+=2;}
	if(dir.z < 0.0) {zmin-=2;} else {zmax+=2;}

	if(xmin < 0)         xmin = 0;
	if(xmax > lengte-1)  xmax = lengte-1;
	if(zmin < 0)         zmin = 0;
	if(zmax > breedte-1) zmax = breedte-1;

	//TODO: follow line over large area

	for(int x=xmin; x<=xmax; x++)
		for(int z=zmin; z<=zmax; z++)
			for(int h=0; h<hoogte; h++)
			{
				float ret = LineTileTest(pos, dirnorm, x, z, h);
				if(ret >= 0.0)
				{
					//printf("Line collision (dx,dz)=(%d,%d); h=%d; dist=%.f\n",
					//	x-xp, z-zp, h, ret);
					return ret;
				}
			}

	return -1.0; //negative = no collision
}

vector<CCollisionData> CCollisionDetector::getCollisions(const CMovingObject *obj)
{
	if(m_FirstUpdate)
	{
		m_FirstUpdate = false;
		calculateTrackBounds();
	}

	vector<CCollisionData> ret;

	//The objects:
	vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);

	//Collision with track bounds
	ret = ObjTrackBoundTest(obj->m_Bodies[0], -obj->m_Velocity);

	//Moving object to tile collisions
	int lengte = theWorld->getTrack()->m_L;
	int breedte = theWorld->getTrack()->m_W;
	int hoogte = theWorld->getTrack()->m_H;
	{
		CVector pos = obj->m_Position;

		int x = (int)(0.5 + (pos.x)/TILESIZE);
		int z = (int)(0.5 + (pos.z)/TILESIZE);

		int xmin = (x-1 < 0)? 0 : x-1;
		int xmax = (x+1 > lengte-1)? lengte-1 : x+1;
		int zmin = (z-1 < 0)? 0 : z-1;
		int zmax = (z+1 > breedte-1)? breedte-1 : z+1;

		for(x=xmin; x<=xmax; x++)
			for(z=zmin; z<=zmax; z++)
				for(int h=0; h<hoogte; h++)
				{
					vector<CCollisionData> tcolls = ObjTileTest(obj, x, z, h);
					for(unsigned int c=0 ; c < tcolls.size(); c++)
						ret.push_back(tcolls[c]);
				}
	}

	//Moving object to moving object collisions
	for(unsigned int i=0; i < objs.size(); i++)
	{
		CMovingObject *o2 = (CMovingObject *)objs[i];

		if(o2 == obj) continue;

		CVector vmean =
			(obj->m_Velocity * o2->getInvMass() + o2->m_Velocity * obj->getInvMass())
			/ (o2->getInvMass() + obj->getInvMass());

		CVector vdiff = o2->m_Velocity - obj->m_Velocity;

		CCollisionData c = ObjObjTest(obj->m_Bodies[0], o2->m_Bodies[0], vdiff, vmean);
		if(c.nor.abs2() < 0.1) continue; //no collision

		ret.push_back(c);
	}


	return ret;
}

const CCollisionFace *CCollisionDetector::getGroundFace(const CVector &pos)
{
	const CCollisionFace *ret = NULL;
	float dmax = 12.0; //if you're higher than this then you must be flying

	//Moving object to tile test
	int lengte = theWorld->getTrack()->m_L;
	int breedte = theWorld->getTrack()->m_W;
	int hoogte = theWorld->getTrack()->m_H;

	int x = (int)(0.5 + (pos.x)/TILESIZE);
	int z = (int)(0.5 + (pos.z)/TILESIZE);

	int xmin = (x-1 < 0)? 0 : x-1;
	int xmax = (x+1 > lengte-1)? lengte-1 : x+1;
	int zmin = (z-1 < 0)? 0 : z-1;
	int zmax = (z+1 > breedte-1)? breedte-1 : z+1;

	for(x=xmin; x<=xmax; x++)
		for(z=zmin; z<=zmax; z++)
			for(int h=0; h<hoogte; h++)
			{
				const CCollisionFace *grnd = getTileGround(x, z, h, pos, dmax);
				if(grnd != NULL)
					ret = grnd;
			}

	//if(ret == NULL) fprintf(stderr, "ground plane = NULL\n");

	return ret;
}

float CCollisionDetector::LineTileTest(const CVector &pos, const CVector &dir, int xtile, int ztile, int htile)
{
	//Tile data
	const STile &theTile = theWorld->getTrack()->m_Track[htile + theWorld->getTrack()->m_H*(ztile + theWorld->getTrack()->m_W*xtile)];
	CVector tilepos = CVector(xtile*TILESIZE, theTile.m_Z*VERTSIZE, ztile*TILESIZE);
	CTileModel *tilemodel = theWorld->getTileModel(theTile.m_Model);

	//relative to the tile
	CVector r = pos - tilepos;

	//Test bounding sphere
	CVector closest = r - r.component(dir);
	if(closest.abs() > tilemodel->m_BSphere_r)
		return -1.0; //negative = no collision

	//rotated back
	CVector d = dir;
	tileRotate(r, 4 - theTile.m_R);
	tileRotate(d, 4 - theTile.m_R);

	float mint = -1.0; //negative = not found

	//Per tile-face
	for(unsigned int tf=0; tf<tilemodel->m_Faces.size(); tf++)
	{
		//fprintf(stderr, "Tileface # %d\n", tf);
		CCollisionFace &theFace = tilemodel->m_Faces[tf];

		float dotpr = d.dotProduct(theFace.nor);
		if(dotpr > -0.0001)
			continue; //invalid plane, or oriented backwards or parallel

		//Calculate intersection point
		float t = (theFace.d - r.dotProduct(theFace.nor)) / dotpr;
		if(t < 0.0) continue; //intersection point behind start point

		//Test if we have had a closer point
		if(mint >= 0.0 && mint < t) continue;

		//Test if this point is inside the face
		if(!theFace.isInside(r + d * t)) continue;

		mint = t;
	} //for tf

	return mint;
}

const CCollisionFace *CCollisionDetector::getTileGround(int xtile, int ztile, int htile, const CVector &pos, float &dmax)
{
	const CCollisionFace *ret = NULL;
	
	//Tile data
	const STile &theTile = theWorld->getTrack()->m_Track[htile + theWorld->getTrack()->m_H*(ztile + theWorld->getTrack()->m_W*xtile)];
	CVector tilepos = CVector(xtile*TILESIZE, theTile.m_Z*VERTSIZE, ztile*TILESIZE);
	CTileModel *tilemodel = theWorld->getTileModel(theTile.m_Model);

	//fprintf(stderr, "Testing tile model %d\n", theTile.m_Model);

	//relative to the tile
	CVector r = pos - tilepos;

	//Test bounding sphere
	if(r.abs() > tilemodel->m_BSphere_r) return NULL;

	//fprintf(stderr, "Test 2: succesful sphere test\n");

	//Per tile-face
	for(unsigned int tf=0; tf<tilemodel->m_Faces.size(); tf++)
	{
		//fprintf(stderr, "Tileface # %d\n", tf);

		//if(tilemodel->m_Faces[tf].isSurface) fprintf(stderr, "it is a surface\n");

		if(!(tilemodel->m_Faces[tf].isSurface)) continue; //not a surface plane

		//fprintf(stderr, "Test 3: it's a surface plane\n");

		if(tilemodel->m_Faces[tf].nor.abs2() < 0.5) continue; //invalid plane
		//fprintf(stderr, "Test 4: it's a valid plane\n");

		//Make a rotated copy
		CCollisionFace theFace = tilemodel->m_Faces[tf];
		for(unsigned int j=0; j<theFace.size(); j++)
			tileRotate(theFace[j], theTile.m_R);

		tileRotate(theFace.nor, theTile.m_R);

		//Calculate distance to plane
		float ddiff = r.dotProduct(theFace.nor) - theFace.d;

		//Test if we are above the plane
		if(ddiff < 0.0)
		{
			continue;
		}
		//fprintf(stderr, "Test 6: not below plane\n");

		//Test if this is the closest plane so far
		if(ddiff > dmax) continue;


		//Test if the closest point is inside the face:
		CVector cpos = r - ddiff * theFace.nor; //the closest point inside the plane
		if(!theFace.isInside(cpos)) continue;

		//fprintf(stderr, "Test 7: WE HAVE A GROUND PLANE!\n");
		dmax = ddiff;
		m_LastGroundFace = theFace;
		m_LastGroundFace.d += tilepos.dotProduct(theFace.nor);
		ret = &m_LastGroundFace;

	} //for tf

	return ret;
}

void CCollisionDetector::calculateTrackBounds()
{
	int minz = 0;
	int maxz = 0;

	int lth = theWorld->getTrack()->m_L;
	int wth = theWorld->getTrack()->m_W;
	int hth = theWorld->getTrack()->m_H;
	for(int x = 0; x < lth; x++)
		for(int y = 0; y < wth; y++)
			for(int h = 0; h < hth; h++)
			{
				int z = theWorld->getTrack()->m_Track[h + hth*(y + wth*x)].m_Z;
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

vector<CCollisionData> CCollisionDetector::ObjTrackBoundTest(const CBody &body, CVector vdiff)
{
	vector<CCollisionData> ret;
	
	CVector p = body.m_Position;
	//CVector v = body.getVelocity();

	const CBound *b = (CBound *)theWorld->getObject(CDataObject::eBound, body.m_Body);
	float r = b->m_BSphere_r;

	if(p.x - r < m_TrackMin.x)
	{
		CCollisionData c;
		c.pos = p;
		c.pos.x = m_TrackMin.x;
		c.nor = CVector(1,0,0);
		c.depth = m_TrackMin.x + r - p.x;
		c.vdiff = vdiff;
		c.vmean = CVector(0,0,0);
		ret.push_back(c);
	}
	if(p.y - r < m_TrackMin.y) //the bottom of the track
	{
		CCollisionData c;
		c.pos = p;
		c.pos.y = m_TrackMin.y;
		c.nor = CVector(0,1,0);
		c.depth = -p.y; //m_TrackMin.y + r - p.y;
		c.vdiff = vdiff;
		c.vmean = CVector(0,0,0);
		ret.push_back(c);
	}
	if(p.z - r < m_TrackMin.z)
	{
		CCollisionData c;
		c.pos = p;
		c.pos.z = m_TrackMin.z;
		c.nor = CVector(0,0,1);
		c.depth = m_TrackMin.z + r - p.z;
		c.vdiff = vdiff;
		c.vmean = CVector(0,0,0);
		ret.push_back(c);
	}
	if(p.x + r > m_TrackMax.x)
	{
		CCollisionData c;
		c.pos = p;
		c.pos.x = m_TrackMax.x;
		c.nor = CVector(-1,0,0);
		c.depth = p.x + r - m_TrackMax.x;
		c.vdiff = vdiff;
		c.vmean = CVector(0,0,0);
		ret.push_back(c);
	}
	if(p.y + r > m_TrackMax.y)
	{
		CCollisionData c;
		c.pos = p;
		c.pos.y = m_TrackMax.y;
		c.nor = CVector(0,-1,0);
		c.depth = p.y + r - m_TrackMax.y;
		c.vdiff = vdiff;
		c.vmean = CVector(0,0,0);
		ret.push_back(c);
	}
	if(p.z + r > m_TrackMax.z)
	{
		CCollisionData c;
		c.pos = p;
		c.pos.z = m_TrackMax.z;
		c.nor = CVector(0,0,-1);
		c.depth = p.z + r - m_TrackMax.z;
		c.vdiff = vdiff;
		c.vmean = CVector(0,0,0);
		ret.push_back(c);
	}

	return ret;
}

CCollisionData CCollisionDetector::ObjObjTest(const CBody &body1, const CBody &body2, CVector vdiff, CVector vmean)
{
	CCollisionData ret;
	ret.nor = CVector(0,0,0); //this will mean that there is no collision

	CVector p1 = body1.m_Position;
	CVector p2 = body2.m_Position;
	const CMatrix &o1 = body1.m_OrientationMatrix;
	const CMatrix &o2 = body2.m_OrientationMatrix;
	const CBound *b1 = (CBound *)theWorld->getObject(CDataObject::eBound, body1.m_Body);
	const CBound *b2 = (CBound *)theWorld->getObject(CDataObject::eBound, body2.m_Body);

	if(!sphereTest(p1, b1, p2, b2)) return ret;

	//fprintf(stderr, "Sphere collision\n");

	/*
	Now do the polyhedra collision detection
	If two convex polyhedra are not penetrating, then there
	must be a plane that separates them.
	We'll just try to find that plane, and if that fails,
	we conclude that a collision has occured

	TODO:
	cache the separation plane
	find the collision point
	*/

	//first try all faces of body 1
	for(unsigned int i=0; i < b1->m_Faces.size(); i++)
	{
		CCollisionFace theFace = b1->m_Faces[i];
		theFace *= o1;
		theFace += p1;
		if(!faceTest(p1, o1, b1, p2, o2, b2, theFace)) return ret;
	}

	//then try all faces of body 2
	for(unsigned int i=0; i < b2->m_Faces.size(); i++)
	{
		CCollisionFace theFace = b2->m_Faces[i];
		theFace *= o2;
		theFace += p2;
		if(!faceTest(p1, o1, b1, p2, o2, b2, theFace)) return ret;
	}

	ret.nor = p1 - p2;
	ret.nor.normalise();
	ret.pos = p1 - b1->m_BSphere_r * ret.nor;
	ret.depth = 0.5 * (b1->m_BSphere_r + b2->m_BSphere_r - (p2-p1).abs());
	ret.vdiff = vdiff;
	ret.vmean = vmean;


	return ret;
}

vector<CCollisionData> CCollisionDetector::ObjTileTest(const CMovingObject *theObj, int xtile, int ztile, int htile)
{
	vector<CCollisionData> ret;

	//Tile data
	const STile &theTile = theWorld->getTrack()->m_Track[htile + theWorld->getTrack()->m_H*(ztile + theWorld->getTrack()->m_W*xtile)];
	CVector tilepos = CVector(xtile*TILESIZE, theTile.m_Z*VERTSIZE, ztile*TILESIZE);
	CTileModel *tilemodel = theWorld->getTileModel(theTile.m_Model);

	//fprintf(stderr, "New tile\n");
	const CBody &theBody = theObj->m_Bodies[0];

	//body position & velocity
	CVector pos = theBody.m_Position;
	CMatrix ori = theBody.m_OrientationMatrix;

	//relative to the tile
	CVector r = pos - tilepos;

	//the collision model of the body
	const CBound *theBound = (CBound *)theWorld->getObject(CDataObject::eBound, theBody.m_Body);

	//fprintf(stderr, "Body %d: r = %s\n", i, CString(r).c_str());

	//Test bounding spheres
	if(!sphereTest(pos, theBound, tilepos, tilemodel)) return ret;

	//fprintf(stderr, "Test 2: succesful sphere test\n");

	//Per tile-face
	for(unsigned int tf=0; tf<tilemodel->m_Faces.size(); tf++)
	{
		//fprintf(stderr, "Tileface # %d\n", tf);

		if(tilemodel->m_Faces[tf].isSurface) continue; //do not collide with surface faces
		if(tilemodel->m_Faces[tf].isWater  ) continue; //do not collide with water   faces

		if(tilemodel->m_Faces[tf].nor.abs2() < 0.5) continue; //invalid plane
		//fprintf(stderr, "Test 4: it's a valid plane\n");

		//1: face-sphere collision test

		//1.1: Make a rotated copy
		CCollisionFace theFace = tilemodel->m_Faces[tf];
		for(unsigned int j=0; j<theFace.size(); j++)
			tileRotate(theFace[j], theTile.m_R);

		tileRotate(theFace.nor, theTile.m_R);

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

		//2: Polygon/bounding faces test:

		//2.1: Rotate + translate the face
		theFace += -r;
		theFace /= ori;

		//2.3: Cut pieces away per plane
		for(unsigned int of=0; of < theBound->m_Faces.size(); of++)
		{
			//fprintf(stderr, "  %d -> Culling with face %d ->", theFace.size(), of);
			theFace.cull(theBound->m_Faces[of], CVector(0,0,0));
			//fprintf(stderr, "%d\n", theFace.size());

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
		cpos += pos; //to absolute coordinates

		//3: Generate collision info
		CCollisionData c;
		c.pos = cpos;
		c.nor = cnor;
		c.depth = penetr_depth;
		c.vdiff = -theObj->m_Velocity;
		c.vmean = CVector(0,0,0);
		ret.push_back(c);

	} //for tf

	return ret;
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

//generic tests:
//return true if there could be a collision

bool CCollisionDetector::sphereTest(const CVector &p1, const CCollisionModel *b1, const CVector &p2, const CCollisionModel *b2)
{
	float abs2 = (p2-p1).abs2();
	float rsum = b1->m_BSphere_r + b2->m_BSphere_r;
	return abs2 < (rsum * rsum);
}

#define MINDDIFF 0.01 //1 cm

bool CCollisionDetector::faceTest(const CVector &p1, const CMatrix &o1, const CBound *b1, const CVector &p2, const CMatrix &o2, const CBound *b2, const CCollisionFace &theFace)
{
	//0 = undetermined, 1 = front, -1 = back
	int side1 = 0, side2 = 0;

	//determine the side of the points of body 1
	for(unsigned int i=0; i < b1->m_Points.size(); i++)
	{
		CVector thePoint = b1->m_Points[i];
		thePoint *= o1;
		thePoint += p1;
		float ddiff = thePoint.dotProduct(theFace.nor) - theFace.d;

		if(ddiff > MINDDIFF)
		{
			if(side1 < 0) return true; //there could be a collision
			side1 = 1;
		}
		else if(ddiff < -MINDDIFF)
		{
			if(side1 > 0) return true; //there could be a collision
			side1 = -1;
		}
	}
	
	//determine the side of the points of body 2
	for(unsigned int i=0; i < b2->m_Points.size(); i++)
	{
		CVector thePoint = b2->m_Points[i];
		thePoint *= o2;
		thePoint += p2;
		float ddiff = thePoint.dotProduct(theFace.nor) - theFace.d;

		if(ddiff > MINDDIFF)
		{
			if(side2 < 0) return true; //there could be a collision
			side2 = 1;
		}
		else if(ddiff < -MINDDIFF)
		{
			if(side2 > 0) return true; //there could be a collision
			side2 = -1;
		}
	}

	if(side1 == side2 && side1 != 0) return true; //both on the same side

	return false; //separation plane -> no collision
}
