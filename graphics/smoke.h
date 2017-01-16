/***************************************************************************
                          smoke.h  -  A smoke particle system
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
#ifndef SMOKE_H
#define SMOKE_H

#include "particlesystem.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CSmoke : public CParticleSystem
{
public:
	CSmoke(unsigned int numParticles);
	virtual ~CSmoke();

	virtual void draw(const CMatrix &cammat) const;

	CVector m_SourcePosition;

	struct SSmokePart : public SParticle
	{
		float age;
		float opacity;
	};

protected:
	virtual SParticle *createParticle();
	virtual void drawParticle(const SParticle *p) const;
	virtual void initNewParticle(SParticle *p);
	virtual void updateParticle(SParticle *p, float dt);

	static CVector m_VertexU, m_VertexV;

	float getRandom();
	unsigned int m_RandomNumbersIndex;
	float m_RandomNumbers[256];
};

#endif
