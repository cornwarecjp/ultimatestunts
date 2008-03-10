/***************************************************************************
                          gui.cpp  -  The graphical user interface: menu's etc.
                             -------------------
    begin                : vr jan 31 2003
    copyright            : (C) 2003 by CJP
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
#include <GL/gl.h>

#include "SDL.h"

#include <libintl.h>
#define _(String) gettext (String)

#include "gui.h"
#include "inputbox.h"
#include "listbox.h"
#include "messagebox.h"
#include "colorselect.h"
#include "fileselect.h"


CGUI::CGUI(CWinSystem *winsys)
{
	//TODO: get conffile options
	m_WinSys = winsys;
	m_ChildWidget = NULL;
	m_in2DMode = false;
}

CGUI::~CGUI(){
}

bool znabled, fnabled;

void CGUI::enter2DMode()
{
	if(m_in2DMode) return;
	m_in2DMode = true;

	znabled = glIsEnabled(GL_DEPTH_TEST);
	if(znabled) glDisable(GL_DEPTH_TEST);
	fnabled = glIsEnabled(GL_FOG);
	if(fnabled) glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	glEnable(GL_SCISSOR_TEST);

	int w = m_WinSys->getWidth();
	int h = m_WinSys->getHeight();
	onResize(0, 0, w, h);
}

void CGUI::leave2DMode()
{
	if(!m_in2DMode) return;
	m_in2DMode = false;

	glEnable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	if(znabled) glEnable(GL_DEPTH_TEST);
	if(fnabled) glEnable(GL_FOG);

	glDisable(GL_SCISSOR_TEST);
}

void CGUI::start()
{
	enter2DMode();

	if(m_ChildWidget != NULL)
		m_WinSys->runLoop(this);

	leave2DMode();
}

int CGUI::onMouseMove(int x, int y, unsigned int buttons)
{
	return m_ChildWidget->onMouseMove(x, y, buttons);
}

int CGUI::onMouseClick(int x, int y, unsigned int buttons)
{
	return m_ChildWidget->onMouseClick(x, y, buttons);
}

int CGUI::onKeyPress(int key)
{
	return m_ChildWidget->onKeyPress(key);
}

int CGUI::onResize(int x, int y, int w, int h)
{
	CWidget::onResize(x, y, w, h);

	//no perspective
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glScissor(0, 0, w, h);

	if(m_ChildWidget == NULL) return 0;

	return m_ChildWidget->onResize(1, 1, w-2, h-2);
}

int CGUI::onRedraw()
{
	//Firt clear the screen
	glDisable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_SCISSOR_TEST);

	return m_ChildWidget->onRedraw();
}

int CGUI::onIdle()
{
	return m_ChildWidget->onIdle();
}

CString CGUI::showSettingBox(const CString &field, const CString &deflt, const CString &metaData, bool *cancelled)
{
	CString title;
	CString options;

	int pos = metaData.inStr('|');
	if(pos < 0)
	{
		title = metaData;
	}
	else
	{
		title = metaData.mid(0, pos);
		options = metaData.mid(pos+1);
	}

	CString options_case = options;
	options.toUpper();

	//Default title = field name
	if(title == "") title = field;

	if(options.inStr("YESNO") >= 0)
	{
		bool ret = showYNMessageBox(title, cancelled);
		if(ret) return "true";
		return "false";
	}

	if(options.inStr("LANGUAGE") >= 0)
	{
		vector<CString> namelist, codelist;
		codelist.push_back("system"); namelist.push_back(_("System's default language"));
		codelist.push_back("de_DE");  namelist.push_back("Deutsch");
		codelist.push_back("en");     namelist.push_back("English");
		codelist.push_back("fr_FR");  namelist.push_back("Français");
		codelist.push_back("hu_HU");  namelist.push_back("Magyar");
		codelist.push_back("nl_NL");  namelist.push_back("Nederlands");
		codelist.push_back("pt_BR");  namelist.push_back("Português Brasileiro");

		CString defltname = deflt;
		for(unsigned int i=0; i < codelist.size(); i++)
			if(codelist[i] == deflt)
			{
				defltname = namelist[i];
				break;
			}

		CString ret = showChoiceBox(_("Language:"), namelist, defltname, cancelled);

		for(unsigned int i=0; i < codelist.size(); i++)
			if(namelist[i] == ret)
				return codelist[i];

		return ret;
	}

	if(options.inStr("DISPLAY") >= 0)
	{
		bool fullscreen = showYNMessageBox(_("Full screen mode?"), cancelled);
		if(cancelled != NULL && *cancelled) return deflt;

		SDL_Rect **modes = SDL_ListModes(NULL, SDL_OPENGL | SDL_FULLSCREEN | SDL_ANYFORMAT);

		if(!fullscreen)
		{
			CString w = showInputBox(_("Enter window width:"), theWinSystem->getWidth(), cancelled);
			if(cancelled != NULL && *cancelled) return deflt;
			CString h = showInputBox(_("Enter window height:"), theWinSystem->getHeight(), cancelled);
			if(cancelled != NULL && *cancelled) return deflt;
			return CString("window:") + w + "x" + h;
		}

		if(modes == NULL || modes == (SDL_Rect **)-1)
		{
			CString w = showInputBox(_("Enter screen resolution width:"), theWinSystem->getWidth(), cancelled);
			if(cancelled != NULL && *cancelled) return deflt;
			CString h = showInputBox(_("Enter screen resolution height:"), theWinSystem->getHeight(), cancelled);
			if(cancelled != NULL && *cancelled) return deflt;
			return CString("fullscreen:") + w + "x" + h;
		}

		//List fullscreen modes:
		vector<CString> list;

		for(unsigned int i=0; modes[i] != NULL; i++)
		{
			CString modename;
			modename.format("%dx%d", 80, modes[i]->w, modes[i]->h);
			list.push_back(modename);
		}
		list.push_back(_("Default resolution"));

		CString defltmode; defltmode.format("%dx%d", 80, theWinSystem->getWidth(), theWinSystem->getHeight());
		CString ret = showChoiceBox(_("Select a screen resolution:"), list, defltmode, cancelled);

		if(ret == list.back()) return "fullscreen";

		return CString("fullscreen:") + ret;
	}

	if(options.inStr("SWITCH") >= 0)
	{
		//Read possible options from options string
		vector<CString> list;

		int pos = options.inStr("SWITCH");
		CString liststring = options_case.mid(pos+6);
		pos = liststring.inStr(':');
		if(pos >= 0)
		{
			liststring = liststring.mid(pos+1);

			while(true)
			{
				pos = liststring.inStr(';');
				if(pos >= 0)
				{
					list.push_back(liststring.mid(0, pos));
					liststring = liststring.mid(pos+1);
				}
				else
				{
					list.push_back(liststring);
					break;
				}
			}
		}

		return showChoiceBox(title, list, deflt, cancelled);
	}

	//Fall-back: a simple input box
	return showInputBox(title, deflt, cancelled);
}

CString CGUI::showChoiceBox(const CString &title, const vector<CString> &options, const CString &deflt, bool *cancelled)
{
	CListBox *listbox = new CListBox;
	listbox->setTitle(title);
	listbox->setOptions(options);

	//Set default
	for(unsigned int i=0; i < options.size(); i++)
		if(options[i] == deflt)
		{
			listbox->setSelected(i);
			break;
		}

	listbox->m_Wrel = 0.5;
	listbox->m_Hrel = 0.5;
	listbox->m_Xrel = 0.25;
	listbox->m_Yrel = 0.25;
	m_ChildWidget->m_Widgets.push_back(listbox);
	m_WinSys->runLoop(this);
	CString ret = options[listbox->getSelected()];
	if(cancelled != NULL)
		*cancelled = listbox->m_Cancelled;

	m_ChildWidget->m_Widgets.resize(m_ChildWidget->m_Widgets.size()-1); //removes messagebox
	delete listbox;

	return ret;
}

CString CGUI::showInputBox(const CString &title, const CString &deflt, bool *cancelled)
{
	CInputBox *inputbox = new CInputBox;
	inputbox->setTitle(title);
	inputbox->m_Text = deflt;
	//determine maximum size
	//unsigned int s = 25;
	//if(title.length() > s) s = title.length();
	//if(deflt.length() > s) s = deflt.length();

	inputbox->m_Wrel = 0.5;
	inputbox->m_Hrel = 0.4;
	inputbox->m_Xrel = 0.25;
	inputbox->m_Yrel = 0.3;
	m_ChildWidget->m_Widgets.push_back(inputbox);
	m_WinSys->runLoop(this);

	CString ret = inputbox->m_Text;
	if(cancelled != NULL)
		*cancelled = inputbox->m_Cancelled;

	if(inputbox->m_Cancelled)
		ret = deflt;

	m_ChildWidget->m_Widgets.resize(m_ChildWidget->m_Widgets.size()-1); //removes inputbox
	delete inputbox;

	return ret;
}

bool CGUI::showYNMessageBox(const CString &title, bool *cancelled)
{
	CMessageBox *messagebox = new CMessageBox;
	messagebox->setTitle(title);
	messagebox->m_Type = CMessageBox::eYesNo;

	messagebox->m_Wrel = 0.5;
	messagebox->m_Hrel = 0.4;
	messagebox->m_Xrel = 0.25;
	messagebox->m_Yrel = 0.3;
	m_ChildWidget->m_Widgets.push_back(messagebox);
	m_WinSys->runLoop(this);
	bool ret = (messagebox->m_Selected == 0);
	if(cancelled != NULL)
		*cancelled = messagebox->m_Cancelled;

	m_ChildWidget->m_Widgets.resize(m_ChildWidget->m_Widgets.size()-1); //removes messagebox
	delete messagebox;

	return ret;
}

void CGUI::showMessageBox(const CString &title)
{
	CMessageBox *messagebox = new CMessageBox;
	messagebox->setTitle(title);
	messagebox->m_Type = CMessageBox::eOK;

	messagebox->m_Wrel = 0.5;
	messagebox->m_Hrel = 0.4;
	messagebox->m_Xrel = 0.25;
	messagebox->m_Yrel = 0.3;
	m_ChildWidget->m_Widgets.push_back(messagebox);
	m_WinSys->runLoop(this);
	m_ChildWidget->m_Widgets.resize(m_ChildWidget->m_Widgets.size()-1); //removes messagebox
	delete messagebox;
}

CVector CGUI::showColorSelect(const CString &title, CVector deflt, bool *cancelled)
{
	CColorSelect *selector = new CColorSelect;
	selector->setTitle(title);
	selector->m_Color = deflt;

	selector->m_Wrel = 0.5;
	selector->m_Hrel = 0.4;
	selector->m_Xrel = 0.25;
	selector->m_Yrel = 0.3;
	m_ChildWidget->m_Widgets.push_back(selector);
	m_WinSys->runLoop(this);

	CVector ret = selector->m_Color;
	if(cancelled != NULL)
		*cancelled = selector->m_Cancelled;

	if(selector->m_Cancelled)
		ret = deflt;

	m_ChildWidget->m_Widgets.resize(m_ChildWidget->m_Widgets.size()-1); //removes selector
	delete selector;

	return ret;
}

CString CGUI::showFileSelect(const CString &title, CString extension, bool *cancelled)
{
	CFileSelect *selector = new CFileSelect;
	selector->setTitle(title);

	selector->setExtension(extension);

	selector->m_Wrel = 0.5;
	selector->m_Hrel = 0.6;
	selector->m_Xrel = 0.25;
	selector->m_Yrel = 0.2;
	m_ChildWidget->m_Widgets.push_back(selector);
	m_WinSys->runLoop(this);

	CString ret = selector->getFullName();
	if(cancelled != NULL)
		*cancelled = selector->m_Cancelled;

	if(selector->m_Cancelled)
		ret = "";

	m_ChildWidget->m_Widgets.resize(m_ChildWidget->m_Widgets.size()-1); //removes selector
	delete selector;

	return ret;
}

