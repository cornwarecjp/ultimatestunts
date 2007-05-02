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
#include "renderwidget.h"
#include "menu.h"

CMainPage::CMainPage() : CGUIPage()
{
	printf("\nInitialising the rendering engine\n");
	m_Renderer = new CTERenderer(theWinSystem);
	m_Camera = new CTECamera();
	m_Renderer->setCamera(m_Camera);

	CRenderWidget *render = new CRenderWidget;
	render->m_Xrel = 0.3;
	render->m_Yrel = 0.0;
	render->m_Wrel = 0.7;
	render->m_Hrel = 0.8;
	render->m_Renderer = m_Renderer;
	m_Widgets.push_back(render);

	CMenu *menu = new CMenu;
	menu->m_Xrel = 0.0;
	menu->m_Yrel = 0.0;
	menu->m_Wrel = 0.3;
	menu->m_Hrel = 0.8;
	menu->m_Lines.push_back(_("Load track"));
	menu->m_Lines.push_back(_("Save track"));
	menu->m_Lines.push_back(_("Import Stunts track"));
	menu->m_Lines.push_back(_("Exit"));
	menu->m_Lines.push_back("");
	menu->m_Lines.push_back(_("Undo"));
	menu->m_Lines.push_back(_("Redo"));
	menu->m_AlignLeft = false;
	m_Widgets.push_back(menu);

	m_DrawBackground = false;

	{
		CReplaceAction *ra = new CReplaceAction;
		ra->m_Tile.m_Model = 1;
		ra->m_Tile.m_Z = 0;
		ra->m_Tile.m_R = 0;
		theTrackDocument->setNewAction(ra);
	}

	m_PrevMouseX = m_PrevMouseY = -1;
	m_TrackCache = NULL;
}

CMainPage::~CMainPage()
{
	delete m_Renderer;
	delete m_Camera;
}

unsigned int CMainPage::getMenuSelection()
{
	return ((CMenu *)(m_Widgets[1]))->m_Selected;
}

void CMainPage::resetCameraPosition()
{
	m_Camera->setTargetPos(theTrackDocument->getCursorPos());
}

int CMainPage::onKeyPress(int key)
{
	SDLMod modState = SDL_GetModState();

	if(m_Widgets.size()==2) //otherwise, maybe there is e.g. a messagebox.
	{
		CVector movement(0,0,0);

		switch(key)
		{
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


		case SDLK_RETURN:
			theTrackDocument->commitAction();
			break;
		case 'r':
			{
				//Change tile rotation
				CReplaceAction *ra = (CReplaceAction *)theTrackDocument->m_Action;
				ra->m_Tile.m_R++;
				if(ra->m_Tile.m_R > 3)
					ra->m_Tile.m_R = 0;

				theTrackDocument->applyAction();
			}
			break;
		case SDLK_TAB:
			//Change tile model
			if(modState & KMOD_SHIFT)
			{
				CReplaceAction *ra = (CReplaceAction *)theTrackDocument->m_Action;
				ra->m_Tile.m_Model--;
				if(ra->m_Tile.m_Model == 0) //0 = delete
					ra->m_Tile.m_Model = int(
					theTrackDocument->m_DataManager->getNumObjects(CDataObject::eTileModel)
					) - 1;
			}
			else
			{
				CReplaceAction *ra = (CReplaceAction *)theTrackDocument->m_Action;
				ra->m_Tile.m_Model++;
				if(ra->m_Tile.m_Model >= int(
					theTrackDocument->m_DataManager->getNumObjects(CDataObject::eTileModel)
					))
						ra->m_Tile.m_Model = 1; //0 = delete
			}
			theTrackDocument->applyAction();
			break;
		case SDLK_DELETE:
			{
				//Change tile model
				CReplaceAction *ra = (CReplaceAction *)theTrackDocument->m_Action;
				int oldModel = ra->m_Tile.m_Model;

				ra->m_Tile.m_Model = 0; //0 = delete

				theTrackDocument->applyAction();
				theTrackDocument->commitAction(); //immediately do it

				ra->m_Tile.m_Model = oldModel;
				theTrackDocument->applyAction(); //apply previous action to new situation
			}
			break;
		case SDLK_INSERT:
			{
				//Change tile insert mode
				CReplaceAction *ra = (CReplaceAction *)theTrackDocument->m_Action;

				ra->m_ClearTile = false;
				theTrackDocument->applyAction();
				theTrackDocument->commitAction(); //immediately do it

				ra->m_ClearTile = true;
				theTrackDocument->applyAction(); //apply previous action to new situation
			}
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

	return CGUIPage::onMouseClick(x, y, buttons);
}

int CMainPage::onMouseMove(int x, int y, unsigned int buttons)
{
	if(m_Widgets.size()==2) //otherwise, maybe there is e.g. a messagebox.
	{
		if(m_Widgets[0]->isInWidget(x,y) && buttons != 0)
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

	return 0;
}
