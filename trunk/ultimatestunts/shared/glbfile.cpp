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
	bool ret = true;
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

		CBinBuffer objectdata = f.readBytes(datalen);
		if(!processObject(type, name, objectdata)) ret = false;
	}

	return ret;
}

bool CGLBFile::processObject(unsigned int type, const CString &name, CBinBuffer &data)
{
	switch(type)
	{
	case 0x0:
		return processGeometry_05(name, data);
		break;
	case 0x1:
		return processGeometry_07(name, data);
		break;
	default:
		printf("Unknown object: type=%x, name=%s\n", type, name.c_str());
		return false;
	}

	return true;
}

bool CGLBFile::processGeometry_05(const CString &name, CBinBuffer &data)
{
	m_Primitives.push_back(SPrimitive());
	SPrimitive &pr = m_Primitives.back();

	unsigned int datapos = 0, pos = 0;

	pr.Name = name;
	CBinBuffer dataheader = data.getBuffer(datapos, 12);
	Uint32 materialsize = dataheader.getUint32(pos);
	Uint32 numVertices = dataheader.getUint32(pos);
	Uint32 numIndices = dataheader.getUint32(pos);

	//printf("Object %s: %d vertices, %d indices\n", pr.Name.c_str(), numVertices, numIndices);

	//Animation settings
	//Default values
	pr.animation.AnimationFlags = 0x0;
	pr.animation.rotationOrigin = CVector();
	pr.animation.rotationVelocity = CVector();

	//Material
	CBinBuffer matbuf = data.getBuffer(datapos, materialsize);
	pos = 0;
	pr.material.ModulationColor.x = float(matbuf.getUint8(pos)) / 255;
	pr.material.ModulationColor.y = float(matbuf.getUint8(pos)) / 255;
	pr.material.ModulationColor.z = float(matbuf.getUint8(pos)) / 255;
	pr.material.Opacity = float(matbuf.getUint8(pos)) / 255;
	pr.material.ReplacementColor.x = float(matbuf.getUint8(pos)) / 255;
	pr.material.ReplacementColor.y = float(matbuf.getUint8(pos)) / 255;
	pr.material.ReplacementColor.z = float(matbuf.getUint8(pos)) / 255;
	matbuf.getUint8(pos); //replacement alpha, not yet used
	pr.material.LODs = matbuf.getUint8(pos);
	pr.material.Reflectance = float(matbuf.getUint8(pos)) / 255;
	pr.material.Emissivity = float(matbuf.getUint8(pos)) / 255;
	matbuf.getFloat8(pos, 16.0/256); //unused pr.material.StaticFriction
	matbuf.getFloat8(pos, 16.0/256); //unused pr.material.DynamicFriction
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
		{pr.material.Texture = -1;}
	else
		{pr.material.Texture = texname.toInt();} //TODO: texture names

	CBinBuffer vertices = data.getBuffer(datapos, 32 * numVertices);
	if(!processVertices(pr, vertices, numVertices)) return false;

	CBinBuffer indices = data.getBuffer(datapos, 4 * numIndices);
	if(!processIndices(pr, indices, numIndices)) return false;

	return true;
}

bool CGLBFile::processGeometry_07(const CString &name, CBinBuffer &data)
{
	m_Primitives.push_back(SPrimitive());
	SPrimitive &pr = m_Primitives.back();

	unsigned int datapos = 0, pos = 0;

	pr.Name = name;
	CBinBuffer dataheader = data.getBuffer(datapos, 16);
	Uint32 animationsize = dataheader.getUint32(pos);
	Uint32 materialsize = dataheader.getUint32(pos);
	Uint32 numVertices = dataheader.getUint32(pos);
	Uint32 numIndices = dataheader.getUint32(pos);

	//printf("Object %s: %d animation bytes, %d material bytes, %d vertices, %d indices\n",
	//	pr.Name.c_str(), animationsize, materialsize, numVertices, numIndices);

	//Animation
	CBinBuffer anibuf = data.getBuffer(datapos, animationsize);
	pos = 0;
	pr.animation.AnimationFlags = anibuf.getUint32(pos);
	pr.animation.rotationOrigin = anibuf.getVector32(pos, 0.001);
	pr.animation.rotationVelocity = anibuf.getVector32(pos, (1000.0 / 128.0) / 65536.0);

	//Material
	CBinBuffer matbuf = data.getBuffer(datapos, materialsize);
	pos = 0;
	pr.material.ModulationColor.x = float(matbuf.getUint8(pos)) / 255;
	pr.material.ModulationColor.y = float(matbuf.getUint8(pos)) / 255;
	pr.material.ModulationColor.z = float(matbuf.getUint8(pos)) / 255;
	pr.material.Opacity = float(matbuf.getUint8(pos)) / 255;
	pr.material.ReplacementColor.x = float(matbuf.getUint8(pos)) / 255;
	pr.material.ReplacementColor.y = float(matbuf.getUint8(pos)) / 255;
	pr.material.ReplacementColor.z = float(matbuf.getUint8(pos)) / 255;
	matbuf.getUint8(pos); //replacement alpha, not yet used
	pr.material.LODs = matbuf.getUint8(pos);
	pr.material.Reflectance = float(matbuf.getUint8(pos)) / 255;
	pr.material.Emissivity = float(matbuf.getUint8(pos)) / 255;
	matbuf.getUint8(pos); //unused
	CString texname;
	for(unsigned int i=pos; i < matbuf.size(); i++)
	{
		if(matbuf[i] == '\0') break;
		texname += char(matbuf[i]);
	}
	if(texname == "")
		{pr.material.Texture = -1;}
	else
		{pr.material.Texture = texname.toInt();} //TODO: texture names


	CBinBuffer vertices = data.getBuffer(datapos, 32 * numVertices);
	if(!processVertices(pr, vertices, numVertices)) return false;

	CBinBuffer indices = data.getBuffer(datapos, 4 * numIndices);
	if(!processIndices(pr, indices, numIndices)) return false;

	return true;
}

bool CGLBFile::processVertices(SPrimitive &pr, CBinBuffer &data, unsigned int numVertices)
{
	if(data.size() < 32 * numVertices)
	{
		printf("End Of Object before end of vertex data\n");
		return false;
	}

	unsigned int pos = 0;
	for(unsigned int i=0; i < numVertices; i++)
	{
		SVertex v;
		v.pos = data.getVector32(pos, 0.001);
		v.nor = data.getVector32(pos, (1.0 / 128.0) / 65536.0);
		v.tex.x = data.getFloat32(pos, 1.0 / 65536.0);
		v.tex.y = data.getFloat32(pos, 1.0 / 65536.0);

		pr.vertex.push_back(v);
	}

	return true;
}

bool CGLBFile::processIndices(SPrimitive &pr, CBinBuffer &data, unsigned int numIndices)
{
	if(data.size() < 4 * numIndices)
	{
		printf("End Of Object before end of index data\n");
		return false;
	}
	
	unsigned int pos = 0;
	for(unsigned int i=0; i < numIndices; i++)
	{
		unsigned int index = data.getUint32(pos);
		if(index >= pr.vertex.size())
		{
			printf("Index %d exceeds vertex array size %d in %s\n",
				index, pr.vertex.size(), pr.Name.c_str());
			
			return false;
		}
		
		pr.index.push_back(index);
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
			CBinBuffer animationdata;
			{
				animationdata += Uint32(pr.animation.AnimationFlags);
				animationdata.addVector32(pr.animation.rotationOrigin, 0.001);
				animationdata.addVector32(pr.animation.rotationVelocity, (1000.0 / 128.0) / 65536.0);
			}

			CBinBuffer materialdata;
			{
				materialdata += Uint8(255*pr.material.ModulationColor.x);
				materialdata += Uint8(255*pr.material.ModulationColor.y);
				materialdata += Uint8(255*pr.material.ModulationColor.z);
				materialdata += Uint8(255*pr.material.Opacity);
				materialdata += Uint8(255*pr.material.ReplacementColor.x);
				materialdata += Uint8(255*pr.material.ReplacementColor.y);
				materialdata += Uint8(255*pr.material.ReplacementColor.z);
				materialdata += Uint8(255);
				materialdata += Uint8(pr.material.LODs);
				materialdata += Uint8(255*pr.material.Reflectance);
				materialdata += Uint8(255*pr.material.Emissivity);
				materialdata += (Uint8)'\0';

				if(pr.material.Texture < 0)
				{
					materialdata += (Uint8)'\0';
					materialdata += (Uint8)'\0';
					materialdata += (Uint8)'\0';
					materialdata += (Uint8)'\0';
				}
				else
				{
					//TODO: more than 10 textures
					materialdata += (Uint8)('0' + pr.material.Texture);
					materialdata += (Uint8)'\0';
					materialdata += (Uint8)'\0';
					materialdata += (Uint8)'\0';
				}
			}

			ObjData += Uint32(animationdata.size());
			ObjData += Uint32(materialdata.size());
			ObjData += Uint32(pr.vertex.size());
			ObjData += Uint32(pr.index.size());
			ObjData += animationdata;
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

		ObjHeader += Uint32(0x1); //type

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

void tesselateSquare(CGLBFile::SPrimitive &pr, unsigned int tess)
{
	//Step 1: find square sizes
	CVector max(-1000.0,-1000.0,-1000.0), min(1000.0, 1000.0, 1000.0);
	for(unsigned int i=0; i < pr.vertex.size(); i++)
	{
		CVector pos = pr.vertex[i].pos;

		if(pos.x > max.x) max.x = pos.x;
		if(pos.y > max.y) max.y = pos.y;
		if(pos.z > max.z) max.z = pos.z;

		if(pos.x < min.x) min.x = pos.x;
		if(pos.y < min.y) min.y = pos.y;
		if(pos.z < min.z) min.z = pos.z;
	}
	CVector size = max - min;
	bool upside = pr.vertex[0].nor.y > 0.0;

	//Step 2: clear primitive
	pr.vertex.clear();
	pr.index.clear();

	//Step 3: create vertices
	for(unsigned int y=0; y <= tess; y++)
	for(unsigned int x=0; x <= tess; x++)
	{
		float xfrac = float(x) / tess;
		float yfrac = float(y) / tess;

		CGLBFile::SVertex vt;

		vt.nor = upside? CVector(0,1,0) : CVector(0,-1,0);
		vt.tex = CVector(xfrac,yfrac,0);
		vt.pos = min + CVector(xfrac*size.x, 0.0, yfrac*size.z);

		pr.vertex.push_back(vt);
	}

	//Step 4: create triangles
	unsigned int base = 0;
	for(unsigned int y=0; y < tess; y++)
	{
	for(unsigned int x=0; x < tess; x++)
	{
		if(upside)
		{
			pr.index.push_back(base);
			pr.index.push_back(base+tess+1);
			pr.index.push_back(base+tess+2);

			pr.index.push_back(base);
			pr.index.push_back(base+tess+2);
			pr.index.push_back(base+1);
		}
		else
		{
			pr.index.push_back(base);
			pr.index.push_back(base+1);
			pr.index.push_back(base+tess+2);

			pr.index.push_back(base);
			pr.index.push_back(base+tess+2);
			pr.index.push_back(base+tess+1);
		}

		base++;
	}
	base++;
	}
}

