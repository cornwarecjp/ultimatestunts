/***************************************************************************
                          fileselect.cpp  -  A File Selection tool
                             -------------------
    begin                : zo feb 04 2007
    copyright            : (C) 2007 by CJP
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

#include "SDL.h"
#include <GL/gl.h>

#include <libintl.h>
#define _(String) gettext (String)

#include "usmacros.h"
#ifdef __CYGWIN__
extern "C" void cygwin_conv_to_full_win32_path(const char *path, char *win32_path);
#endif

#include "fileselect.h"
#include "console.h"

#include "cfile.h"

CString CFileSelect::m_Directory = getAbsDir("./");

CFileSelect::CFileSelect()
{
	loadConsoleFont();

	m_Cancelled = false;
	m_Title = "Title";
	m_Selected = eNone;

	m_Menu.m_Wrel = 0.8;
	m_Menu.m_Hrel = 0.6;
	m_Menu.m_Xrel = 0.1;
	m_Menu.m_Yrel = 0.2;

	m_Menu.m_AlignLeft = true;
	updateDirListing();
}

CFileSelect::~CFileSelect(){
}

void CFileSelect::setTitle(const CString &title)
{
	m_Title = title;
}

void CFileSelect::updateDirListing()
{
	m_Menu.m_Lines.clear();
	m_Menu.m_Selected = 0;

	vector<CString> files = getDirContents(m_Directory);
	for(unsigned int i=0; i < files.size(); i++)
		if(
			dirExists(m_Directory + files[i]) ||
			files[i].right(m_Extension.length()).toLower() == m_Extension)
				m_Menu.m_Lines.push_back(files[i]);

}

int CFileSelect::selectEntry()
{
	CString filename = m_Menu.m_Lines[m_Menu.m_Selected];
	if(dirExists(m_Directory + filename))
	{
		enterDirectory(filename);
		return WIDGET_REDRAW;
	}

	printf("Found non-directory: \"%s\"\n", (m_Directory + filename).c_str());
	m_File = filename;
	m_Cancelled = false;
	return WIDGET_QUIT;
}

void CFileSelect::enterDirectory(const CString &dirname)
{
	if(dirname == ".") return;

	if(dirname == "..")
	{
		if(m_Directory == "/") return;

#ifdef __CYGWIN__
		if(m_Directory == "/cygdrive/") return;
#endif

		int parentlen = -1;
		if(m_Directory.length() > 2 && m_Directory[m_Directory.length()-1] == '/')
		{
			CString dir = m_Directory.mid(0, m_Directory.length()-1);
			for(unsigned int i=0; i < dir.length(); i++)
				if(dir[i] == '/')
					parentlen = i+1;
		}

		if(parentlen > 0)
		{
			m_Directory = m_Directory.mid(0, parentlen);
			updateDirListing();
			return;
		}
	}

	m_Directory += dirname + "/";
	updateDirListing();
}

int CFileSelect::onKeyPress(int key)
{
	if(key == SDLK_ESCAPE)
	{
		m_Cancelled = true;
		return WIDGET_QUIT;
	}

	if(key == SDLK_RETURN)
		return selectEntry();

	m_Menu.onKeyPress(key);
	return WIDGET_REDRAW;
}

int CFileSelect::onMouseMove(int x, int y, unsigned int buttons)
{
	if(m_Menu.isInWidget(x, y))
		m_Menu.onMouseMove(x, y, buttons);

	m_Selected = eNone;
	if(y - m_Y < 0.2*m_H)
	{
		m_Selected = eCancel;
	}

	return WIDGET_REDRAW;
}

int CFileSelect::onMouseClick(int x, int y, unsigned int buttons)
{
	onMouseMove(x, y, buttons);

	if(m_Menu.isInWidget(x, y))
	{
		int menureturn = m_Menu.onMouseClick(x, y, buttons);
		if((menureturn & WIDGET_QUIT) != 0)
			return selectEntry();
	}

	if(m_Selected == eCancel)
		{m_Cancelled = true; return WIDGET_QUIT;}

	return 0;
}

int CFileSelect::onResize(int x, int y, int w, int h)
{
	return CWidget::onResize(x, y, w, h) | m_Menu.onResize(
		int(x+m_Menu.m_Xrel*w),
		int(y+m_Menu.m_Yrel*h),
		int(m_Menu.m_Wrel*w),
		int(m_Menu.m_Hrel*h)
		);
}

int CFileSelect::onRedraw()
{
	CWidget::onRedraw();

	glColor4f(0, 0, 0, 0.7);
	drawBackground();
	glColor4f(1,1,1,1);

	//centered
	glPushMatrix();
	glTranslatef(m_W/2, m_H/2, 0);

	//Title
	theConsoleFont->enable();

	glPushMatrix();
	glTranslatef(-0.5*theConsoleFont->getFontW()*m_Title.length(), 0.4*m_H, 0);
	theConsoleFont->drawString(m_Title);
	glPopMatrix();

	glPushMatrix();
	{
		int numChars = int(float(m_W) / theConsoleFont->getFontW());
		CString dispDir = m_Directory;

#ifdef __CYGWIN__
		if(dispDir == "/cygdrive/")
			{dispDir = "";}
		else
		{
			//User-friendly win32-style path in Cygwin
			char win32path[4096];
			cygwin_conv_to_full_win32_path(dispDir.c_str(), win32path);
			dispDir = win32path;
		}
#endif

		if(int(dispDir.length()) > numChars && numChars > 4)
			dispDir = "..." + dispDir.right(numChars-4);

		glTranslatef(-0.5*theConsoleFont->getFontW()*dispDir.length(), 0.3*m_H, 0);
		theConsoleFont->drawString(dispDir);
	}
	glPopMatrix();

	theConsoleFont->disable();

	glPopMatrix(); //Centered


	//Cancel button
	glPushMatrix();
	glTranslatef(0.2*m_W, 0.1*m_H, 0);

	if(m_Selected == eCancel)
		{glColor3f(1, 0.9, 0);}
	else
		{glColor3f(1, 1, 1);}
	theConsoleFont->enable();
	theConsoleFont->drawString(_("Cancel"));
	theConsoleFont->disable();
	glPopMatrix();


	m_Menu.onRedraw();

	return 0;
}

