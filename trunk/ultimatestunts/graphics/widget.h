/***************************************************************************
                          widget.h  -  Base-class for all GUI widgets
                             -------------------
    begin                : wo dec 15 2004
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

#ifndef WIDGET_H
#define WIDGET_H

#define WIDGET_QUIT  		1
#define WIDGET_REDRAW		2
#define WIDGET_CANCELLED	4

class CWidget {
public:
	CWidget();
	virtual ~CWidget();

	virtual int onMouseMove(int x, int y, unsigned int buttons);
	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onKeyPress(int key);
	virtual int onResize(int x, int y, int w, int h);
	virtual int onRedraw();
	virtual int onIdle();

	bool isInWidget(int x, int y);

	int getX(){return m_X;}
	int getY(){return m_Y;}
	int getW(){return m_W;}
	int getH(){return m_H;}

	float m_Xrel, m_Yrel, m_Wrel, m_Hrel; //only for use by parent widget
protected:
	int m_X, m_Y, m_W, m_H;

	void drawBackground();
};

#endif
