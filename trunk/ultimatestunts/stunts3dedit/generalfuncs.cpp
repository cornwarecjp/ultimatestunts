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
			"1: *.glt / *.gl file\n"
			"2: *.glb file\n"
			"3: 3dto3d smooth *.raw file\n"
			"4: 3D Studio *.3ds file\n");
		switch(getInput("Your choice: ").toInt())
		{
			case 1:
				obj2.loadGLTFile(getInput("Enter filename: ")); break;
			case 2:
				obj2.loadGLBFile(getInput("Enter filename: ")); break;
			case 3:
				obj2.loadRAWFile(getInput("Enter filename: ")); break;
			case 4:
				obj2.load3DSFile(getInput("Enter filename: ")); break;
			default:
				printf("Please choose between 1, 2, 3 and 4\n");
		}
		graphobj->merge(obj2, lods);
	}
	else
	{
		printf("Opening file. Choose from:\n"
			"1: *.glt / *.gl file\n"
			"2: *.glb file\n"
			"3: 3dto3d smooth *.raw file\n"
			"4: 3D Studio *.3ds file\n");
		switch(getInput("Your choice: ").toInt())
		{
			case 1:
				graphobj->loadGLTFile(getInput("Enter filename: ")); break;
			case 2:
				graphobj->loadGLBFile(getInput("Enter filename: ")); break;
			case 3:
				graphobj->loadRAWFile(getInput("Enter filename: ")); break;
			case 4:
				graphobj->load3DSFile(getInput("Enter filename: ")); break;
			default:
				printf("Please choose between 1, 2, 3 and 4\n");
		}
	}
	graphobj->render(VisibleLODs);
}

void saveFunc()
{
	printf("Saving file. Choose from:\n"
		"1: *.glb file\n"
		"2: *.glt file\n");
	switch(getInput("Your choice: ").toInt())
	{
	case 1:
		graphobj->saveGLBFile(getInput("Saving *.glb file.\nEnter filename: "));
		break;
	case 2:
		graphobj->saveGLTFile(getInput("Saving *.glt file.\nEnter filename: "));
		break;
	}

	return;
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
