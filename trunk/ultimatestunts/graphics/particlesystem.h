/***************************************************************************
                          particlesystem.h  -  A particle system base class
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
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
namespace std {}
using namespace std;

#include "vector.h"
#include "matrix.h"

#include "texture.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CParticleSystem
{
public:
	CParticleSystem(unsigned int numParticles);
	virtual ~CParticleSystem();

	void update(float dt, bool newParticle=true);
	virtual void draw(const CMatrix &cammat) const;

	struct SParticle
	{
		CVector pos, vel;
		float rotation, size;
	};

	bool m_Enabled;
	CTexture *m_Texture;

protected:
	vector<SParticle *> m_Particles;
	unsigned int m_OldestParticle;

	virtual SParticle *createParticle();

	virtual void initNewParticle(SParticle *p);
	virtual void updateParticle(SParticle *p, float dt);
	virtual void drawParticle(const SParticle *p) const;
};

#endif
