/***************************************************************************
                          replaceactionwidget.cpp  -  Widget interface for CReplaceAction
                             -------------------
    begin                : zo jul 15 2007
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
#include <GL/gl.h>
#include "SDL.h"

#include <libintl.h>
#define _(String) gettext (String)
#define N_(String1, String2, n) ngettext ((String1), (String2), (n))

#include "trackdocument.h"
#include "winsystem.h"

#include "replaceactionwidget.h"

#include "replaceaction.h"

CReplaceActionWidget::CReplaceActionWidget()
{
	m_TileSelect.m_Xrel = 0.0;
	m_TileSelect.m_Yrel = 0.2;
	m_TileSelect.m_Wrel = 1.0;
	m_TileSelect.m_Hrel = 0.8;

	m_IconBar.m_Xrel = 0.0;
	m_IconBar.m_Yrel = 0.0;
	m_IconBar.m_Wrel = 1.0;
	m_IconBar.m_Hrel = 0.2;
	m_IconBar.addIcon(CIconList::eRotateTile, 0.1, _("Rotate"));
	m_IconBar.addIcon(CIconList::eCombineTiles, 1.1, _("Combine"));
	m_IconBar.addIcon(CIconList::eOK, 1.1, _("Apply"));
	m_IconBar.addIcon(CIconList::eDelete, 1.1, _("Delete"));

	CReplaceAction *ra = new CReplaceAction;
	ra->m_Tile.m_Model = 1;
	ra->m_Tile.m_Z = 0;
	ra->m_Tile.m_R = 0;
	m_Action = ra;
}


CReplaceActionWidget::~CReplaceActionWidget()
{
	delete m_Action;
}

void CReplaceActionWidget::rotate()
{
	CReplaceAction *ra = (CReplaceAction *)m_Action;
	ra->m_Tile.m_R++;
	if(ra->m_Tile.m_R > 3)
		ra->m_Tile.m_R = 0;

	theTrackDocument->setNewAction(ra);
}

void CReplaceActionWidget::insert()
{
	CReplaceAction *ra = (CReplaceAction *)m_Action;

	//Change tile insert mode
	ra->m_ClearTile = false;
	theTrackDocument->setNewAction(ra);
	theTrackDocument->commitAction(); //immediately do it

	ra->m_ClearTile = true;
	theTrackDocument->setNewAction(ra);
}

void CReplaceActionWidget::apply()
{
	theTrackDocument->commitAction();
}

void CReplaceActionWidget::del()
{
	CReplaceAction *ra = (CReplaceAction *)m_Action;

	//Change tile model
	int oldModel = ra->m_Tile.m_Model;

	ra->m_Tile.m_Model = 0; //0 = delete
	theTrackDocument->setNewAction(ra);

	theTrackDocument->applyAction();
	theTrackDocument->commitAction(); //immediately do it

	ra->m_Tile.m_Model = oldModel;
	theTrackDocument->setNewAction(ra);
}

int CReplaceActionWidget::onKeyPress(int key)
{
	SDLMod modState = SDL_GetModState();

	CReplaceAction *ra = (CReplaceAction *)m_Action;

	switch(key)
	{
	case SDLK_RETURN:
		apply();
		return WIDGET_REDRAW;
	case 'r':
		rotate();
		return WIDGET_REDRAW;
	case SDLK_TAB:
		//Change tile model
		if(modState & KMOD_SHIFT)
		{
			ra->m_Tile.m_Model--;
			if(ra->m_Tile.m_Model == 0) //0 = delete
				ra->m_Tile.m_Model = int(
				theTrackDocument->m_DataManager->getNumObjects(CDataObject::eTileModel)
				) - 1;
		}
		else
		{
			ra->m_Tile.m_Model++;
			if(ra->m_Tile.m_Model >= int(
				theTrackDocument->m_DataManager->getNumObjects(CDataObject::eTileModel)
				))
					ra->m_Tile.m_Model = 1; //0 = delete
		}
		m_TileSelect.setSelection(ra->m_Tile.m_Model);
		theTrackDocument->setNewAction(ra);
		return WIDGET_REDRAW;
	case SDLK_DELETE:
		del();
		return WIDGET_REDRAW;
	case SDLK_INSERT:
		insert();
		return WIDGET_REDRAW;
	}

	return 0;
}

int CReplaceActionWidget::onMouseClick(int x, int y, unsigned int buttons)
{
	CReplaceAction *ra = (CReplaceAction *)m_Action;

	int ret = 0;
	if(m_TileSelect.isInWidget(x, y))
	{
		if(m_TileSelect.onMouseClick(x, y, buttons) && WIDGET_QUIT)
		{
			ra->m_Tile.m_Model = m_TileSelect.getSelection();
			theTrackDocument->setNewAction(ra);
		}
		ret = WIDGET_REDRAW;
	}
	else if(m_IconBar.isInWidget(x, y))
	{
		if(m_IconBar.onMouseClick(x, y, buttons) && WIDGET_QUIT)
		{
			switch(m_IconBar.m_Selected)
			{
			case CIconList::eRotateTile:
				rotate(); break;
			case CIconList::eCombineTiles:
				insert(); break;
			case CIconList::eOK:
				apply(); break;
			case CIconList::eDelete:
				del(); break;
			default:
				break;
			}
		}

		ret = WIDGET_REDRAW;
	}

	return ret;
}

int CReplaceActionWidget::onMouseMove(int x, int y, unsigned int buttons)
{
	int ret = 0;
	if(m_TileSelect.isInWidget(x, y))
		{ret = m_TileSelect.onMouseMove(x, y, buttons);}
	else if(m_IconBar.isInWidget(x, y))
		{ret = m_IconBar.onMouseMove(x, y, buttons);}

	return ret;
}

int CReplaceActionWidget::onResize(int x, int y, int w, int h)
{
	CWidget::onResize(x, y, w, h);
	return m_TileSelect.onResize(
			int(x+m_TileSelect.m_Xrel*w),
			int(y+m_TileSelect.m_Yrel*h),
			int(m_TileSelect.m_Wrel*w),
			int(m_TileSelect.m_Hrel*h)
			)
		| m_IconBar.onResize(
			int(x+m_IconBar.m_Xrel*w),
			int(y+m_IconBar.m_Yrel*h),
			int(m_IconBar.m_Wrel*w),
			int(m_IconBar.m_Hrel*h)
			);
}

int CReplaceActionWidget::onIdle()
{
	return m_TileSelect.onIdle() | m_IconBar.onIdle();
}

int CReplaceActionWidget::onRedraw()
{
	int ret = m_TileSelect.onRedraw() | m_IconBar.onRedraw();

	//Selection could be changed e.g. when a different track has been loaded
	//  between two redraws

	CReplaceAction *ra = (CReplaceAction *)m_Action;
	if(int(m_TileSelect.getSelection()) != ra->m_Tile.m_Model)
	{
		ra->m_Tile.m_Model = m_TileSelect.getSelection();
		theTrackDocument->setNewAction(ra);
	}

	return ret;
}

