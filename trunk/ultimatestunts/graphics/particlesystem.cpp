/***************************************************************************
                          particlesystem.cpp  -  A particle system base class
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
#include <cstdio>

#include "particlesystem.h"

CParticleSystem::CParticleSystem(unsigned int numParticles)
{
	m_Particles.resize(numParticles);

	//Put this in every derived class' constructor
	//It can't be here because the base class
	//constructor cannot call the virtual function
	//createParticle correctly
	/*
	for(unsigned int i=0; i < numParticles; i++)
	{
		m_Particles[i] = createParticle();
		m_Particles[i]->size = -1.0; //not initialized
	}
	*/

	m_OldestParticle = 0;
}


CParticleSystem::~CParticleSystem()
{
	for(unsigned int i=0; i < m_Particles.size(); i++)
		delete m_Particles[i];
}

void CParticleSystem::update(float dt, bool newParticle)
{
	if(!m_Enabled) return;

	if(newParticle)
	{
		initNewParticle(m_Particles[m_OldestParticle]);

		m_OldestParticle++;
		if(m_OldestParticle >= m_Particles.size())
			m_OldestParticle = 0;
	}

	for(unsigned int i=0; i < m_Particles.size(); i++)
		updateParticle(m_Particles[i], dt);
}

void CParticleSystem::draw(const CMatrix &cammat) const
{
	if(!m_Enabled) return;

	for(unsigned int i=0; i < m_Particles.size(); i++)
		drawParticle(m_Particles[i]);
}

CParticleSystem::SParticle *CParticleSystem::createParticle()
{
	return new SParticle;
}

void CParticleSystem::initNewParticle(SParticle *p)
{
}

void CParticleSystem::updateParticle(SParticle *p, float dt)
{
}

void CParticleSystem::drawParticle(const SParticle *p) const
{
}


