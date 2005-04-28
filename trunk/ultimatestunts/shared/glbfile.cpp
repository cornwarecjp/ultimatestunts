/***************************************************************************
                          glbfile.cpp  -  Interface for the GLB file format
                             -------------------
    begin                : di feb 1 2005
    copyright            : (C) 2005 by CJP
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

#include "glbfile.h"
#include "datafile.h"

CGLBFile::CGLBFile(){
}
CGLBFile::~CGLBFile(){
}

bool CGLBFile::load(const CString &filename)
{
	CDataFile f(filename);

	CBinBuffer magicbytes = f.readBytes(4);
	if(magicbytes.size() < 4)
	{
		printf("Error: %s does not contain any data\n", filename.c_str());
		return false;
	}

	if(magicbytes[0] != '\0' ||
		magicbytes[1] != 'G' ||
		magicbytes[2] != 'L' ||
		magicbytes[3] != 'B')
	{
		printf("Magic bytes are wrong. %s is not a GLB file\n", filename.c_str());
		return false;
	}


	m_Primitives.clear();

	//read objects
	while(true)
	{
		unsigned int pos=0;
		CBinBuffer objheader = f.readBytes(8);
		if(objheader.size() < 8) break; //no more objects
		Uint32 type = objheader.getUint32(pos);
		Uint32 namelen = objheader.getUint32(pos);
		CBinBuffer namebuf = f.readBytes(namelen + 4);
		CString name;
		for(unsigned int i=0; i < namelen; i++)
		{
			if(namebuf[i] == '\0') break;
			name += (char)namebuf[i];
		}
		pos = namelen;
		Uint32 datalen = namebuf.getUint32(pos);

		switch(type)
		{
		case 0:
		{
			m_Primitives.push_back(SPrimitive());
			SPrimitive &pr = m_Primitives.back();

			pr.Name = name;
			CBinBuffer dataheader = f.readBytes(12);
			pos = 0;
			Uint32 materialsize = dataheader.getUint32(pos);
			Uint32 numVertices = dataheader.getUint32(pos);
			Uint32 numIndices = dataheader.getUint32(pos);

			//printf("Object %s: %d vertices, %d indices\n", pr.Name.c_str(), numVertices, numIndices);

			//Material
			CBinBuffer matbuf = f.readBytes(materialsize);
			pos = 0;
			pr.ModulationColor.x = float(matbuf.getUint8(pos)) / 255;
			pr.ModulationColor.y = float(matbuf.getUint8(pos)) / 255;
			pr.ModulationColor.z = float(matbuf.getUint8(pos)) / 255;
			pr.Opacity = float(matbuf.getUint8(pos)) / 255;
			pr.ReplacementColor.x = float(matbuf.getUint8(pos)) / 255;
			pr.ReplacementColor.y = float(matbuf.getUint8(pos)) / 255;
			pr.ReplacementColor.z = float(matbuf.getUint8(pos)) / 255;
			matbuf.getUint8(pos); //replacement alpha, not yet used
			pr.LODs = matbuf.getUint8(pos);
			pr.Reflectance = float(matbuf.getUint8(pos)) / 255;
			pr.Emissivity = float(matbuf.getUint8(pos)) / 255;
			pr.StaticFriction = matbuf.getFloat8(pos, 16.0/256);
			pr.DynamicFriction = matbuf.getFloat8(pos, 16.0/256);
			matbuf.getUint8(pos); //unused
			matbuf.getUint8(pos); //unused
			matbuf.getUint8(pos); //unused
			CString texname;
			for(unsigned int i=pos; i < matbuf.size(); i++)
			{
				if(matbuf[i] == '\0') break;
				texname += char(matbuf[i]);
			}
			if(texname == "")
				{pr.Texture = -1;}
			else
				{pr.Texture = texname.toInt();} //TODO: texture names

			CBinBuffer vertices = f.readBytes(32 * numVertices);
			if(vertices.size() < 32 * numVertices)
			{
				printf("EOF before end of vertex data\n");
				return false;
			}
			pos = 0;
			for(unsigned int i=0; i < numVertices; i++)
			{
				SVertex v;
				v.pos = vertices.getVector32(pos, 0.001);
				v.nor = vertices.getVector32(pos, (1.0 / 128.0) / 65536.0);
				v.tex.x = vertices.getFloat32(pos, 1.0 / 65536.0);
				v.tex.y = vertices.getFloat32(pos, 1.0 / 65536.0);

				pr.vertex.push_back(v);
			}

			CBinBuffer indices = f.readBytes(4 * numIndices);
			if(indices.size() < 4 * numIndices)
			{
				printf("EOF before end of index data\n");
				return false;
			}
			pos = 0;
			for(unsigned int i=0; i < numIndices; i++)
			{
				unsigned int index = indices.getUint32(pos);
				if(index >= pr.vertex.size())
				{
					printf("Index %d exceeds vertex array size %d in %s\n",
						index, pr.vertex.size(), pr.Name.c_str());
					
					return false;
				}
				
				pr.index.push_back(index);
			}

			break;
		}
		default: //unknown object type
			printf("Unknown object %s\n", name.c_str());
			f.readBytes(datalen);
		}
	}

	return true;
}

void CGLBFile::save(const CString &filename)
{
	CDataFile f(filename, true);

	CBinBuffer header;
	header += (Uint8)'\0';
	header += (Uint8)'G';
	header += (Uint8)'L';
	header += (Uint8)'B';

	f.writeBytes(header);

	for(unsigned int i=0; i < m_Primitives.size(); i++)
	{
		SPrimitive &pr = m_Primitives[i];

		CBinBuffer ObjHeader, ObjData;

		{
			CBinBuffer materialdata;
			materialdata += Uint8(255*pr.ModulationColor.x);
			materialdata += Uint8(255*pr.ModulationColor.y);
			materialdata += Uint8(255*pr.ModulationColor.z);
			materialdata += Uint8(255*pr.Opacity);
			materialdata += Uint8(255*pr.ReplacementColor.x);
			materialdata += Uint8(255*pr.ReplacementColor.y);
			materialdata += Uint8(255*pr.ReplacementColor.z);
			materialdata += Uint8(255);
			materialdata += Uint8(pr.LODs);
			materialdata += Uint8(255*pr.Reflectance);
			materialdata += Uint8(255*pr.Emissivity);
			materialdata.addFloat8(pr.StaticFriction, 16.0/256);
			materialdata.addFloat8(pr.DynamicFriction, 16.0/256);
			materialdata += (Uint8)'\0';
			materialdata += (Uint8)'\0';
			materialdata += (Uint8)'\0';

			if(pr.Texture < 0)
			{
				materialdata += (Uint8)'\0';
				materialdata += (Uint8)'\0';
				materialdata += (Uint8)'\0';
				materialdata += (Uint8)'\0';
			}
			else
			{
				//TODO: more than 10 textures
				materialdata += (Uint8)('0' + pr.Texture);
				materialdata += (Uint8)'\0';
				materialdata += (Uint8)'\0';
				materialdata += (Uint8)'\0';
			}

			ObjData += Uint32(materialdata.size());
			ObjData += Uint32(pr.vertex.size());
			ObjData += Uint32(pr.index.size());
			ObjData += materialdata;

			//vertex array
			for(unsigned int j=0; j < pr.vertex.size(); j++)
			{
				ObjData.addVector32(pr.vertex[j].pos, 0.001);
				ObjData.addVector32(pr.vertex[j].nor, (1.0 / 128.0) / 65536.0);
				ObjData.addFloat32(pr.vertex[j].tex.x, 1.0 / 65536.0);
				ObjData.addFloat32(pr.vertex[j].tex.y, 1.0 / 65536.0);
			}

			//indices
			for(unsigned int j=0; j < pr.index.size(); j++)
				ObjData += Uint32(pr.index[j]);
		}

		ObjHeader += Uint32(0); //type

		//Object name
		unsigned int numPadding = 1 + (4 - (1+pr.Name.length()) % 4);

		ObjHeader += Uint32(pr.Name.length() + numPadding);

		for(unsigned int j=0; j < pr.Name.length(); j++)
			ObjHeader += Uint8(pr.Name[j]);
		for(unsigned int j=0; j < numPadding; j++)
			ObjHeader += (Uint8)'\0';

		ObjHeader += Uint32(ObjData.size());

		f.writeBytes(ObjHeader);
		f.writeBytes(ObjData);
	}
}

