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
	CWidget()
		{m_W = m_H = 0;}

	virtual ~CWidget(){;}

	virtual int onMouseMove(int x, int y){return 0;}
	virtual int onMouseClick(int x, int y, unsigned int buttons){return 0;}
	virtual int onKeyPress(int key){return 0;}
	virtual int onResize(int w, int h){m_W = w; m_H = h; return WIDGET_REDRAW;}
	virtual int onRedraw(){return 0;}

	int getW(){return m_W;}
	int getH(){return m_H;}
	int m_X, m_Y;
protected:
	int m_W, m_H;
};

#endif
