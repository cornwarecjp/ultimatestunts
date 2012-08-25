/***************************************************************************
                          mainpage.cpp  -  The main page of the trackedit interface
                             -------------------
    begin                : do dec 12 2006
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
#include "SDL.h"

#include <libintl.h>
#define _(String) gettext (String)
#define N_(String1, String2, n) ngettext ((String1), (String2), (n))

#include "mainpage.h"

#include "trackdocument.h"
#include "replaceaction.h"

#include "winsystem.h"

CMainPage::CMainPage() : CGUIPage()
{
	printf("\nInitialising the rendering engine\n");
	m_Renderer = new CTERenderer;
	m_Camera = new CTECamera();
	m_Renderer->setCamera(m_Camera);

	m_RenderWidget = new CRenderWidget;
	m_RenderWidget->m_Xrel = 0.3;
	m_RenderWidget->m_Yrel = 0.05;
	m_RenderWidget->m_Wrel = 0.7;
	m_RenderWidget->m_Hrel = 0.8;
	m_RenderWidget->m_Renderer = m_Renderer;
	m_Widgets.push_back(m_RenderWidget);

	m_HintLabel = new CLabel;
	m_HintLabel->m_Xrel = 0.3;
	m_HintLabel->m_Yrel = 0.0;
	m_HintLabel->m_Wrel = 0.7;
	m_HintLabel->m_Hrel = 0.05;
	m_HintLabel->m_Text = "";
	m_Widgets.push_back(m_HintLabel);

	m_FilanemeLabel = new CLabel;
	m_FilanemeLabel->m_Xrel = 0.3;
	m_FilanemeLabel->m_Yrel = 0.85;
	m_FilanemeLabel->m_Wrel = 0.7;
	m_FilanemeLabel->m_Hrel = 0.05;
	m_FilanemeLabel->m_Text = "[new file]";
	m_Widgets.push_back(m_FilanemeLabel);

	m_Compass = new CCompass;
	m_Compass->m_Xrel = 0.0;
	m_Compass->m_Yrel = 0.0;
	m_Compass->m_Wrel = 0.3;
	m_Compass->m_Hrel = 0.1;
	m_Compass->setCamera(m_Camera);
	m_Widgets.push_back(m_Compass);

	m_ActionWidget = new CReplaceActionWidget;
	m_ActionWidget->m_Xrel = 0.0;
	m_ActionWidget->m_Yrel = 0.1;
	m_ActionWidget->m_Wrel = 0.3;
	m_ActionWidget->m_Hrel = 0.8;
	m_Widgets.push_back(m_ActionWidget);

	m_IconBar = new CIconBar;
	m_IconBar->m_Xrel = 0.0;
	m_IconBar->m_Yrel = 0.9;
	m_IconBar->m_Wrel = 1.0;
	m_IconBar->m_Hrel = 0.1;
	m_IconBar->addIcon(CIconList::eLoad       , 0.1, _("Load track"));
	m_IconBar->addIcon(CIconList::eImport     , 1.1, _("Import Stunts track"));
	m_IconBar->addIcon(CIconList::eSave       , 1.1, _("Save track"));
	m_IconBar->addIcon(CIconList::eQuit       , 1.1, _("Exit"));
	m_IconBar->addIcon(CIconList::eUndo       , 2.1, _("Undo"));
	m_IconBar->addIcon(CIconList::eRedo       , 1.1, _("Redo"));
	m_IconBar->addIcon(CIconList::eTiles      , 2.1, _("Change tiles collection"));
	m_IconBar->addIcon(CIconList::eScenery    , 1.1, _("Change scenery settings"));
	m_IconBar->addIcon(CIconList::eResizeTrack, 1.1, _("Resize the track"));
	m_Widgets.push_back(m_IconBar);

	m_NormalNumWidgets = m_Widgets.size();

	m_DrawBackground = false;

	enableActionWidgetAction();

	m_PrevMouseX = m_PrevMouseY = -1;
	m_TrackCache = NULL;
}

CMainPage::~CMainPage()
{
	//Widgets will be deleted automagically by base class

	delete m_Renderer;
	delete m_Camera;
}

CIconList::eIconID CMainPage::getSelection()
{
	return m_IconBar->m_Selected;
}

void CMainPage::resetCameraPosition()
{
	m_Camera->setTargetPos(theTrackDocument->getCursorPos());
}

void CMainPage::updateDocInfo()
{
	m_FilanemeLabel->m_Text = theTrackDocument->m_Trackname;

	m_Title = "";
}

void CMainPage::enableActionWidgetAction()
{
	theTrackDocument->setNewAction(m_ActionWidget->m_Action);
}

int CMainPage::onKeyPress(int key)
{
	SDLMod modState = SDL_GetModState();

	if(m_Widgets.size()==m_NormalNumWidgets) //otherwise, maybe there is e.g. a messagebox.
	{
		CVector movement(0,0,0);

		//Action-based key handling
		int action_ret = m_ActionWidget->onKeyPress(key);
		if(action_ret != 0) return action_ret;

		//Otherwise, default key handling
		switch(key)
		{
		case SDLK_ESCAPE:
			return WIDGET_CANCELLED | WIDGET_QUIT;
		case SDLK_PAGEUP:
			movement.y = 1;
			break;
		case SDLK_PAGEDOWN:
			movement.y = -1;
			break;
		case SDLK_LEFT:
			movement.x = -1;
			break;
		case SDLK_RIGHT:
			movement.x = 1;
			break;
		case SDLK_UP:
			movement.z = -1;
			break;
		case SDLK_DOWN:
			movement.z = 1;
			break;
		case 'z':
		case 'Z':
			if(modState & KMOD_CTRL)
			{
				if(modState & KMOD_SHIFT)
					{theTrackDocument->redo();} //Ctrl-Shift-z = redo`
				else
					{theTrackDocument->undo();} //Ctrl-z = undo
			}
			break;
		}

		if(movement.abs2() > 0.1)
		{
			int dx=0,dy=0,dz=0;

			if(fabsf(movement.y) > 0.1)
			{
				dy = (movement.y>0) ? 1 : -1;
			}
			else
			{
				movement.y = 0;

				movement *= m_Camera->getOrientation();

				if(fabsf(movement.x) > fabsf(movement.z))
				{
					dx = (movement.x>0) ? 1 : -1;
				}
				else
				{
					dz = (movement.z>0) ? 1 : -1;
				}
			}

			theTrackDocument->moveCursor(
				theTrackDocument->getCursorX()+dx,
				theTrackDocument->getCursorY()+dy,
				theTrackDocument->getCursorZ()+dz
				);

			theTrackDocument->applyAction(); //action could be changed

			m_Camera->setTargetPos(theTrackDocument->getCursorPos());

			return WIDGET_REDRAW;
		}

		return 0;
	}

	return CGUIPage::onKeyPress(key);
}

int CMainPage::onMouseClick(int x, int y, unsigned int buttons)
{
	m_PrevMouseX = -1;
	m_PrevMouseY = -1;

	if(m_ActionWidget->isInWidget(x, y))
		return m_ActionWidget->onMouseClick(x, y, buttons);

	if(m_Compass->isInWidget(x, y))
		if(m_Compass->onMouseClick(x, y, buttons) & WIDGET_QUIT)
		{
			int dx=0,dy=0,dz=0;

			switch(m_Compass->m_Selection)
			{
			case CCompass::eXup:
				dx = 1;
				break;
			case CCompass::eXdn:
				dx = -1;
				break;
			case CCompass::eYup:
				dy = 1;
				break;
			case CCompass::eYdn:
				dy = -1;
				break;
			case CCompass::eZup:
				dz = 1;
				break;
			case CCompass::eZdn:
				dz = -1;
				break;
			case CCompass::eNone:
				break;
			}

			theTrackDocument->moveCursor(
				theTrackDocument->getCursorX()+dx,
				theTrackDocument->getCursorY()+dy,
				theTrackDocument->getCursorZ()+dz
				);

			theTrackDocument->applyAction(); //action could be changed

			m_Camera->setTargetPos(theTrackDocument->getCursorPos());

			return WIDGET_REDRAW;
		}

	return CGUIPage::onMouseClick(x, y, buttons);
}

int CMainPage::onMouseMove(int x, int y, unsigned int buttons)
{
	if(m_Widgets.size()==m_NormalNumWidgets) //otherwise, maybe there is e.g. a messagebox.
	{
		if(m_ActionWidget->isInWidget(x, y))
			return m_ActionWidget->onMouseMove(x, y, buttons);

		if(m_Compass->isInWidget(x, y))
			return m_Compass->onMouseMove(x, y, buttons);

		if(m_RenderWidget->isInWidget(x,y) && buttons != 0)
		{
			if(m_PrevMouseX < 0 || m_PrevMouseY < 0)
			{
				m_PrevMouseX = x;
				m_PrevMouseY = y;
			}
	
			int dx = x - m_PrevMouseX;
			int dy = y - m_PrevMouseY;
			if(dx >  10) dx =  10;
			if(dx < -10) dx = -10;
			if(dy >  10) dy =  10;
			if(dy < -10) dy = -10;
	
			if(buttons & SDL_BUTTON(1))
			{
				m_Camera->turnRight(-0.01*dx);
				m_Camera->turnUp(-0.01*dy);
			}
	
			if(buttons & SDL_BUTTON(3))
			{
				m_Camera->zoomOut(1.0 - 0.005*dy);
			}
	
			m_PrevMouseX = x;
			m_PrevMouseY = y;
	
			return WIDGET_REDRAW;
		}
		else
		{
			m_PrevMouseX = -1;
			m_PrevMouseY = -1;
		}

		int ret = CGUIPage::onMouseMove(x, y, buttons);
	
		m_HintLabel->m_Text = m_IconBar->m_Description;
	
		return ret;
	}

	return CGUIPage::onMouseMove(x, y, buttons);
}

int CMainPage::onIdle()
{
	CEditTrack *track = theTrackDocument->getDisplayedTrack();
	if(track != m_TrackCache)
	{
		m_TrackCache = track;
		return WIDGET_REDRAW;
	}

	return CGUIPage::onIdle();
}
