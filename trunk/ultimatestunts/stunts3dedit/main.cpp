/***************************************************************************
                          main.cpp  -  3D-Edit main
                             -------------------
    begin                : wo nov 20 23:23:28 CET 2002
    copyright            : (C) 2002 by CJP
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

//Key configuration:
enum eKeyconf {
	eExit = '\e',
	eLoad = 'F',
	eSave = 'f',

	ePrimitive = 'p',
	eVertex = 'v',

	eChange = 'c',
	eRotate = 'r',
	eScale = 's',
	eMirror = 'm',
	eTranslate = 't',

	eRotateTexture = 'R',
	eTranslateTexture = 'T',

	eHelp = 'h'
};

//Common files
#include "lconfig.h"
#include "cstring.h"

//Graphics stuff
#include "winsystem.h"
#include "editrenderer.h"
#include "editcamera.h"

#include "editgraphobj.h"
#include "textureloader.h"

CWinSystem *winsys;
CEditRenderer *renderer;
CEditCamera *camera;

CEditGraphObj *graphobj;
CTextureLoader *texloader;

CString topdir;

//State variables:
int curr_primitive = 0, curr_vertex = 0;

CString getInput(CString question="")
{
	printf("%s", question.c_str());
	char input[80];
	scanf("%s", input);
	return input;
}

bool mainloop()
{
	bool ret = true;

	const Uint8 *keystate = winsys->getKeyState();


	if(winsys->wasPressed(eExit))
		ret = false;

	if(winsys->wasPressed(eLoad))
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

	if(winsys->wasPressed(eSave))
		graphobj->saveToFile(
			topdir + getInput("Saving *.gl file.\nEnter filename: "));

	if(winsys->wasPressed(ePrimitive))
	{
		for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
			printf("%d: %s\n", i, graphobj->m_Primitives[i].m_Name.c_str());
		curr_primitive = getInput("Select primitive: ").toInt();
		curr_vertex = 0;
	}

	if(winsys->wasPressed(eVertex))
	{
		CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
		unsigned int nv = pr.m_Vertex.size();
		printf("%s has %d vertices:\n", pr.m_Name.c_str(), nv);
		for(unsigned int i=0; i<nv; i++)
			printf("\t%d: %f,%f,%f\n", i,
				pr.m_Vertex[i].pos.x,
				pr.m_Vertex[i].pos.y,
				pr.m_Vertex[i].pos.z);
		curr_vertex = getInput("Select vertex: ").toInt();
	}

	if(winsys->wasPressed(eChange))
	{
		CVertex &vt = graphobj->m_Primitives[curr_primitive].m_Vertex[curr_vertex];
		printf("Pos: %f,%f,%f\n", vt.pos.x, vt.pos.y, vt.pos.z);
		printf("Nor: %f,%f,%f\n", vt.nor.x, vt.nor.y, vt.nor.z);
		printf("Col: %f,%f,%f\n", vt.col.x, vt.col.y, vt.col.z);
		printf("Tex: %f,%f\n", vt.tex.x, vt.tex.y);

		CString answ = getInput("Give new position: ");
		if(answ != "-")
			vt.pos = answ.toVector();

		answ = getInput("Give new normal: ");
		if(answ != "-")
			vt.nor = answ.toVector();

		answ = getInput("Give new color: ");
		if(answ != "-")
			vt.col = answ.toVector();

		answ = getInput("Give new texcoord: ");
		if(answ != "-")
			vt.tex = answ.toVector();

		graphobj->render();
	}

	if(winsys->wasPressed(eRotate))
	{
		bool scene = getInput("Do you want to rotate the entire scene? ") == "y";
		CVector x = getInput("Enter the new position of the x-axis: ").toVector();
		CVector y = getInput("Enter the new position of the y-axis: ").toVector();
		CVector z = getInput("Enter the new position of the z-axis: ").toVector();

		CMatrix m;
		m.setElement(0,0, x.x); m.setElement(0,1, y.x); m.setElement(0,2, z.x);
		m.setElement(1,0, x.y); m.setElement(1,1, y.y); m.setElement(1,2, z.y);
		m.setElement(2,0, x.z); m.setElement(2,1, y.z); m.setElement(2,2, z.z);

		if(scene)
		{
			for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
			{
				CPrimitive &pr = graphobj->m_Primitives[i];
				for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
				{
					CVertex &vt = pr.m_Vertex[j];
					vt.pos *= m;
					vt.nor *= m;
				}
			}
		}
		else
		{
				CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
				for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
				{
					CVertex &vt = pr.m_Vertex[j];
					vt.pos *= m;
					vt.nor *= m;
				}
		}

		graphobj->render();
	}

	if(winsys->wasPressed(eScale))
	{
		bool scene = getInput("Do you want to autoscale the entire scene? ") == "y";
		float maxs = getInput("Enter new size: ").toFloat();

		float min = 1.0e10;
		float max = -min;
		if(scene)
		{
			for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
			{
				CPrimitive &pr = graphobj->m_Primitives[i];
				for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
				{
					CVector &p = pr.m_Vertex[j].pos;
					if(p.x > max) max = p.x;
					if(p.y > max) max = p.y;
					if(p.z > max) max = p.z;
					if(p.x < min) min = p.x;
					if(p.y < min) min = p.y;
					if(p.z < min) min = p.z;
				}
			}
		}
		else
		{
			CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
			for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
			{
				CVector &p = pr.m_Vertex[j].pos;
				if(p.x > max) max = p.x;
				if(p.y > max) max = p.y;
				if(p.z > max) max = p.z;
				if(p.x < min) min = p.x;
				if(p.y < min) min = p.y;
				if(p.z < min) min = p.z;
			}
		}

		float sf = maxs / (max-min);
		printf("Scaling with scalefactor %f\n", sf);

		if(scene)
		{
			for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
			{
				CPrimitive &pr = graphobj->m_Primitives[i];
				for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
					pr.m_Vertex[j].pos *= sf;
			}
		}
		else
		{
				CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
				for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
					pr.m_Vertex[j].pos *= sf;
		}

		graphobj->render();
	}

	if(winsys->wasPressed(eMirror))
	{
		bool normals = getInput("Do you want to mirror the normals? ") == "y";
		bool vertices = getInput("Do you want to mirror the vertices? ") == "y";
		CPrimitive &pr = graphobj->m_Primitives[curr_primitive];

		if(normals)
		{
			for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
			{
				CVertex &vt = pr.m_Vertex[j];
				if(normals)
					vt.nor = -vt.nor;
			}
		}

		if(vertices)
		{
			unsigned int size =pr.m_Vertex.size();
			for(unsigned int j=0; j<size/2; j++)
			{
				CVertex vt = pr.m_Vertex[j];  //swapping the vertices; putting in reverse order
				pr.m_Vertex[j] = pr.m_Vertex[size-j-1];
				pr.m_Vertex[size-j-1] = vt;
			}
		}

		graphobj->render();
	}

	if(winsys->wasPressed(eTranslate))
	{
		bool scene = getInput("Do you want to translate the entire scene? ") == "y";
		CVector v = getInput("Enter translation vector: ").toVector();

		if(scene)
		{
			for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
			{
				CPrimitive &pr = graphobj->m_Primitives[i];
				for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
					pr.m_Vertex[j].pos += v;
			}
		}
		else
		{
			CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
			for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
				pr.m_Vertex[j].pos += v;
		}

		graphobj->render();
	}

	if(winsys->wasPressed(eRotateTexture))
	{
		CVector x = getInput("Enter the new position of the x-axis: ").toVector();
		CVector y = getInput("Enter the new position of the y-axis: ").toVector();

		CMatrix m;
		m.setElement(0,0, x.x); m.setElement(0,1, y.x);
		m.setElement(1,0, x.y); m.setElement(1,1, y.y);

		CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
		for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
			pr.m_Vertex[j].tex *= m;

		graphobj->render();
	}

	if(winsys->wasPressed(eTranslateTexture))
	{
		//TODO: implement this
	}

	if(winsys->wasPressed(eHelp))
		printf("Keys:\n"
			"Esc: Exit\n"
			"%c: Load\n"
			"%c: Save\n"
			"%c: Primitive\n"
			"%c: Vertex\n"
			"%c: Change\n"
			"%c: Rotate\n"
			"%c: Scale\n"
			"%c: Mirror\n"
			"%c: Translate\n"
			"%c: Rotate Texture\n"
			"%c: Translate Texture\n"
			"%c: Help\n",
			eLoad, eSave, ePrimitive, eVertex, eChange, eRotate, eScale,
			eMirror, eTranslate, eRotateTexture, eTranslateTexture, eHelp
		);

	if(winsys->wasPressed(SDLK_END))
		camera->flipCameraCenter();

	if(keystate[SDLK_PAGEUP])
		camera->incrDist(-1.0);
	if(keystate[SDLK_PAGEDOWN])
		camera->incrDist(1.0);
	if(keystate[SDLK_LEFT])
		camera->incrXAngle(0.1);
	if(keystate[SDLK_RIGHT])
		camera->incrXAngle(-0.1);
	if(keystate[SDLK_UP])
		camera->incrYAngle(0.1);
	if(keystate[SDLK_DOWN])
		camera->incrYAngle(-0.1);

	renderer->update();
	return ret;
}

int main(int argc, char *argv[])
{
	CLConfig conffile(argc, argv);
	if(!conffile.setFilename("ultimatestunts.conf"))
	{
		printf("Error: could not read ultimatestunts.conf\n"); return 1;
		//TODO: create a default one
	} else {printf("Using ultimatestunts.conf\n");}

	printf("\nCreating a window\n");
	winsys = new CWinSystem("Stunts 3D Edit", conffile);

	topdir = conffile.getValue("files", "datadir");
	if(topdir != "" && topdir[topdir.length()-1] != '/')
		topdir += '/';
	printf("Filenames are relative to \"%s\"\n", topdir.c_str());

	printf("\nLoading textures from textures.dat\n");
	texloader = new CTextureLoader(conffile, "textures.dat");

	CString fn = getInput("Enter the filename: ");
	graphobj = new CEditGraphObj;
	printf("Loading graphic object...\n");
	graphobj->loadFromFile(topdir + fn, texloader->m_TexArray);
	printf("...done\n");

	printf("\nInitialising the rendering engine\n");
	renderer = new CEditRenderer(conffile);
	camera = new CEditCamera();
	renderer->setCamera(camera);
	renderer->setGraphobj(graphobj);

	printf("\nEntering mainloop\n");
	winsys->runLoop(mainloop, true); //true: swap buffers
	printf("\nLeaving mainloop\n");

	delete renderer;
	delete winsys; //Important; don't remove: this calls SDL_Quit!!!

	printf("\nProgram finished succesfully\n");

  return EXIT_SUCCESS;
}
