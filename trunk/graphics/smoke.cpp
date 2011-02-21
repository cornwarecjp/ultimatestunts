/***************************************************************************
                          smoke.cpp  -  A smoke particle system
                             -------------------
    begin                : ma jan 14 2008
    copyright            : (C) 2008 by CJP
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
#include <cstdlib>
#include <GL/gl.h>

#include "smoke.h"

CTexture *_SmokeTexture = NULL;

CVector CSmoke::m_VertexU, CSmoke::m_VertexV;;

CSmoke::CSmoke(unsigned int numParticles): CParticleSystem(numParticles)
{
	for(unsigned int i=0; i < numParticles; i++)
	{
		m_Particles[i] = createParticle();
		m_Particles[i]->size = -1.0; //not initialized
	}

	if(_SmokeTexture == NULL)
	{
		_SmokeTexture = new CTexture(NULL);

		CParamList plist;
		SParameter p;
		p.name = "sizex";
		p.value = 512;
		plist.push_back(p);
		p.name = "sizey";
		p.value = 512;
		plist.push_back(p);
		p.name = "smooth";
		p.value = true; //TODO?
		plist.push_back(p);

		_SmokeTexture->load("misc/smoke.rgba", plist);
	}

	m_Texture = _SmokeTexture;
	m_Enabled = false;

	//Fill random number source
	m_RandomNumbersIndex = 0;
	for(unsigned int i=0; i < 256; i++)
		m_RandomNumbers[i] = double(rand()) / double(RAND_MAX);
}


CSmoke::~CSmoke()
{
}

CParticleSystem::SParticle *CSmoke::createParticle()
{
	return new SSmokePart;
}

void CSmoke::draw(const CMatrix &cammat) const
{
	glDepthMask(GL_FALSE);

	m_Texture->draw();

	m_VertexU = cammat * CVector(1,0,0);
	m_VertexV = cammat * CVector(0,1,0);
	CVector normal = cammat * CVector(0,0,1);

	glNormal3f(normal.x, normal.y, normal.z);

	glBegin(GL_QUADS);
	CParticleSystem::draw(cammat);
	glEnd();

	glDepthMask(GL_TRUE);
}

void CSmoke::drawParticle(const SParticle *p) const
{
	if(p->size < 0.0) return; //not initialized

	SSmokePart *s = (SSmokePart *)p;

	CVector u = 0.5*p->size * m_VertexU;
	CVector v = 0.5*p->size * m_VertexV;

	CVector v1 = p->pos -u -v;
	CVector v2 = p->pos +u -v;
	CVector v3 = p->pos +u +v;
	CVector v4 = p->pos -u +v;

	static float color[] = {1.0, 1.0, 1.0, 1.0};
	color[3] = s->opacity;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(v1.x, v1.y, v1.z);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(v2.x, v2.y, v2.z);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(v3.x, v3.y, v3.z);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(v4.x, v4.y, v4.z);
}

void CSmoke::initNewParticle(SParticle *p)
{
	SSmokePart *s = (SSmokePart *)p;

	s->pos = m_SourcePosition;
	s->size = 0.5 + 1.0*getRandom();
	s->rotation = 0.0;
	s->vel = CVector(
		0.5*getRandom()-0.25,
		0.5 + 1.0*getRandom(),
		0.5*getRandom()-0.25
		);
	s->age = 0.0;
	s->opacity = 1.0;
}

void CSmoke::updateParticle(SParticle *p, float dt)
{
	SSmokePart *s = (SSmokePart *)p;

	if(s->size < 0.0) return; //uninitialized particle

	s->pos += dt * s->vel;
	s->size += 1.0 * dt;
	s->age += dt;
	s->opacity = exp(-1.0 * s->age);
}

float CSmoke::getRandom()
{
	m_RandomNumbersIndex = (m_RandomNumbersIndex+1) & 0xff; //modulo 256
	return m_RandomNumbers[m_RandomNumbersIndex];
}

