/***************************************************************************
                          generalfuncs.cpp  -  General functions for stunts3dedit
                             -------------------
    begin                : vr okt 8 2004
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

void loadFunc()
{
	if(getInput("Do you want to merge the file as a LOD (y/n)? ") == "y")
	{
		CString lods = getInput("Enter the LODs of the new objects: ");

		CEditGraphObj obj2;
		printf("Opening file. Choose from:\n"
			"1: *.gl file\n"
			"2: 3dto3d smooth *.raw file\n");
		switch(getInput("Your choice: ").toInt())
		{
			case 1:
				obj2.loadFromFile(
					topdir + getInput("Enter filename: "),
					texloader->m_TexArray); break;
			case 2:
				obj2.import_raw(
					topdir + getInput("Enter filename: "),
					texloader->m_TexArray); break;
			default:
				printf("Please choose between 1 and 2\n");
		}
		graphobj->merge(obj2, lods);
	}
	else
	{
		printf("Opening file. Choose from:\n"
			"1: *.gl file\n"
			"2: 3dto3d smooth *.raw file\n");
		switch(getInput("Your choice: ").toInt())
		{
			case 1:
				graphobj->loadFromFile(
					topdir + getInput("Enter filename: "),
					texloader->m_TexArray); break;
			case 2:
				graphobj->import_raw(
					topdir + getInput("Enter filename: "),
					texloader->m_TexArray); break;
			default:
				printf("Please choose between 1 and 2\n");
		}
	}
	graphobj->render(VisibleLODs);
}

void saveFunc()
{
	graphobj->saveToFile(topdir + getInput("Saving *.gl file.\nEnter filename: "));
}

void settingsFunc()
{
	VisibleLODs = getInput("Set visible LODs: ");
	graphobj->render(VisibleLODs);
}

void helpFunc()
{
	printf("Keys:\n"
		"Esc: Exit\n"
		"%c: Load\n"
		"%c: Save\n"
		"%c: Settings\n"
		"%c: Primitive\n"
		"%c: Vertex\n"
		"%c: New primitive or vertex\n"
		"%c: Change\n"
		"%c: Change primitive\n"
		"%c: Duplicate primitive\n"
		"%c: Set as collision object\n"
		"%c: Rotate\n"
		"%c: Scale\n"
		"%c: Mirror\n"
		"%c: Translate\n"
		"%c: Rotate Texture\n"
		"%c: Translate Texture\n"
		"%c: Help\n",
		eLoad, eSave, eSettings, ePrimitive, eVertex, eNew, eChange,
		eChangePrimitive, eDuplicatePrimitive, eSetCollision, eRotate, eScale,
		eMirror, eTranslate, eRotateTexture, eTranslateTexture,
		eHelp
	);
}
