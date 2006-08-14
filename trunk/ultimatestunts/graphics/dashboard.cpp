/***************************************************************************
                          dashboard.cpp  -  A dashboard for cars etc.
                             -------------------
    begin                : ma jan 30 2006
    copyright            : (C) 2006 by CJP
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

#ifndef M_PI
#define M_PI 3.1415926536
#endif

#include <GL/gl.h>

#include "font.h"
#include "console.h"

#include "world.h"
#include "car.h"
#include "carinput.h"

#include "dashboard.h"

CDashboard::CDashboard(CDataManager *manager, unsigned int movObjID) :
	m_BackgroundTexture(manager),
	m_SteerTexture(manager),
	m_VelGaugeTex(manager),
	m_RPMGaugeTex(manager)
{
	m_MovObjID = movObjID;

	CMovingObject *mo = theWorld->getMovingObject(movObjID);
	if(mo->getType() == CMessageBuffer::car)
	{
		m_Info = ((CCar *)mo)->m_Dashboard;

		if(m_Info.background_tex != "")
			m_BackgroundTexture.load(m_Info.background_tex, CParamList());

		if(m_Info.steer_tex != "")
			m_SteerTexture.load(m_Info.steer_tex, CParamList());

		if(m_Info.analog_vel_tex != "")
			m_VelGaugeTex.load(m_Info.analog_vel_tex, CParamList());

		if(m_Info.analog_rpm_tex != "")
			m_RPMGaugeTex.load(m_Info.analog_rpm_tex, CParamList());
	}
}

CDashboard::~CDashboard()
{
}

void CDashboard::draw(unsigned int w, unsigned int h, eShowMode mode)
{
	CMovingObject *theObj = theWorld->getMovingObject(m_MovObjID);
	if(theObj->getType() != CMessageBuffer::car) return;

	CCar *theCar = (CCar *)theObj;

	glPushMatrix();
	glScalef(w, w, 1.0);
	
	//The background
	if(mode == eFull && m_BackgroundTexture.isLoaded())
	{
		m_BackgroundTexture.draw();

		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(0,0);
		glTexCoord2f(1,0);
		glVertex2f(1,0);
		glTexCoord2f(1,1);
		glVertex2f(1,m_Info.background_hth);
		glTexCoord2f(0,1);
		glVertex2f(0,m_Info.background_hth);
		glEnd();
	}

	//Analog meters:
	if(m_Info.analog_vel_pos.abs2() > 0.01)
	{
		float x = m_Info.analog_vel_pos.x;
		float y = m_Info.analog_vel_pos.y;
		float r = m_Info.analog_vel_rad;

		if(mode != eFull)
		{
			x = 0.1;
			y = 0.1;
			r = 0.07;
		}

		if(m_VelGaugeTex.isLoaded())
		{

			m_VelGaugeTex.draw();

			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2f(x-r,y-r);
			glTexCoord2f(1,0);
			glVertex2f(x+r,y-r);
			glTexCoord2f(1,1);
			glVertex2f(x+r,y+r);
			glTexCoord2f(0,1);
			glVertex2f(x-r,y+r);
			glEnd();
		}

		float needle_angle =
			m_Info.analog_vel_an0 +
			(m_Info.analog_vel_an1 - m_Info.analog_vel_an0) *
			theCar->m_Velocity.abs() / m_Info.analog_vel_max;

		needle_angle *= (M_PI/180.0); //to radians

		float xend = x + 0.8*r * sin(needle_angle);
		float yend = y + 0.8*r * cos(needle_angle);

		glColor3f(1,0,0);
		glBindTexture(GL_TEXTURE_2D, 0); //no texture
		glBegin(GL_LINES);
		glVertex2f(x,y);
		glVertex2f(xend,yend);
		glEnd();
		glColor3f(1,1,1);
	}

	if(m_Info.analog_rpm_pos.abs2() > 0.01)
	{
		float x = m_Info.analog_rpm_pos.x;
		float y = m_Info.analog_rpm_pos.y;
		float r = m_Info.analog_rpm_rad;

		if(mode != eFull)
		{
			x = 0.9;
			y = 0.1;
			r = 0.07;
		}

		if(m_RPMGaugeTex.isLoaded())
		{
			m_RPMGaugeTex.draw();

			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2f(x-r,y-r);
			glTexCoord2f(1,0);
			glVertex2f(x+r,y-r);
			glTexCoord2f(1,1);
			glVertex2f(x+r,y+r);
			glTexCoord2f(0,1);
			glVertex2f(x-r,y+r);
			glEnd();
		}

		float engineRPS = theCar->m_Engine.m_MainAxisW * theCar->m_Engine.getGearRatio();

		float needle_angle =
			m_Info.analog_rpm_an0 +
			(m_Info.analog_rpm_an1 - m_Info.analog_rpm_an0) *
			engineRPS / m_Info.analog_rpm_max;

		needle_angle *= (M_PI/180.0); //to radians

		float xend = x + 0.8*r * sin(needle_angle);
		float yend = y + 0.8*r * cos(needle_angle);

		glColor3f(1,0,0);
		glBindTexture(GL_TEXTURE_2D, 0); //no texture
		glBegin(GL_LINES);
		glVertex2f(x,y);
		glVertex2f(xend,yend);
		glEnd();
		glColor3f(1,1,1);
	}

	//The steer
	if(mode == eFull && m_SteerTexture.isLoaded())
	{
		float r = m_Info.steer_rad;

		glPushMatrix();
		glTranslatef(m_Info.steer_pos.x, m_Info.steer_pos.y, 0.0);
		glRotatef(-m_Info.steer_ang * theCar->m_DesiredSteering,  0.0,0.0,1.0);

		m_SteerTexture.draw();

		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(-r,-r);
		glTexCoord2f(1,0);
		glVertex2f(r,-r);
		glTexCoord2f(1,1);
		glVertex2f(r,r);
		glTexCoord2f(0,1);
		glVertex2f(-r,r);
		glEnd();

		glPopMatrix();
	}

	glPopMatrix();

	theConsoleFont->enable();

	//The message in the middle:
	CString message;
	float msgAlpha = 1.0;
	if(theObj->m_IncomingMessages.size() > 0)
	{
		//remove all messages but the last:
		if(theObj->m_IncomingMessages.size() > 1)
		{
			theObj->m_IncomingMessages[0] = theObj->m_IncomingMessages.back();
			theObj->m_IncomingMessages.resize(1); //delete all messages except the last
		}

		float racingTime = theWorld->m_LastTime;
		float msgAge = racingTime - theObj->m_IncomingMessages[0].m_SendTime;
		float maxAge = 1.0 + 0.1 * theObj->m_IncomingMessages[0].m_Message.length();
		float fadeAge = 1.0;

		/*
		printf("msg \"%s\" age = %.2f - %.2f = %.2f\n",
			theObj->m_IncomingMessages[0].m_Message.c_str(),
			racingTime, theObj->m_IncomingMessages[0].m_SendTime, msgAge);
		*/

		if(msgAge < maxAge)
		{
			message = theObj->m_IncomingMessages[0].m_Message;

			msgAge = msgAge - maxAge + fadeAge;
			if(msgAge > 0.0)
			{
				msgAlpha = 1.0 - msgAge / fadeAge;
			}
		}
		else
		{
			theObj->m_IncomingMessages.clear(); //also delete the last message
		}
	}

	if(message.length() > 0)
	{
		float size = 2.0; //character size
		glPushMatrix();
		glTranslatef(
			0.5*(w - message.length() * size * theConsoleFont->getFontW()),
			0.5*(h + theConsoleFont->getFontH()),
			0);
		glScalef(size,size,size);
		glColor4f(1,1,1,msgAlpha);
		theConsoleFont->drawString(message);
		glColor4f(1,1,1,1);
		glPopMatrix();
	}

	//The time
	if(theWorld->m_LastTime > 0.0)
	{
		CString time = CString().fromTime(theWorld->m_LastTime);

		float size = 2.0; //character size
		glPushMatrix();
		glTranslatef(
			0.5*(w - time.length() * size * theConsoleFont->getFontW()),
			h - 2.5*theConsoleFont->getFontH(),
			0);
		glScalef(size,size,size);
		theConsoleFont->drawString(time);
		glPopMatrix();
	}

	theConsoleFont->disable();
}
