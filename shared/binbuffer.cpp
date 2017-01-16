/***************************************************************************
                          binbuffer.cpp  -  binary buffer class
                             -------------------
    begin                : di dec 17 2002
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
#include <cstdio>
 
#include "binbuffer.h"

//Code coming from binbuffer.h
//Originally written by bones

/*
CBinBuffer & CBinBuffer::operator += (const char unsigned & c) {
	this->push_back((Uint8) c); 	
	return (*this);
}
*/

CBinBuffer & CBinBuffer::operator += (const char unsigned * c) {                // c-string
	for (int i=0; c[i] != 0; i++) { this->push_back((Uint8) c[i]); } 	
	return (*this);
}

CBinBuffer & CBinBuffer::operator = (const char unsigned * c) {
	this->empty();
	(*this)+=c;
	return (*this);
}

CBinBuffer & CBinBuffer::operator += (const CBinBuffer & bb) {
	for (int unsigned i=0;i<bb.size();i++) this->push_back(bb[i]);   // FIXME: slow
	return (*this);
}

CBinBuffer CBinBuffer::getBuffer(int unsigned &pos, unsigned int maxlen) const
{
	unsigned int s = size();
	if(s==0) return CBinBuffer();
	
	if(maxlen >= s) maxlen = s-1;

	CBinBuffer ret;
	ret.resize(maxlen);
	for (int unsigned i=0;i<maxlen;i++)
	{
		ret[i] = (*this)[pos];   // FIXME: slow
		pos++;
	}

	return ret;
}

//Uint8:
CBinBuffer & CBinBuffer::operator += (const Uint8 & i)
{
	this->push_back(i);
	return *this;
}

Uint8 CBinBuffer::getUint8(int unsigned &pos) const
{
	//Removed: there is no exception handling in Ultimate Stunts.
	//Now the caller is responsible for checking the
	//length of the binary buffer.
	//if (pos > this->size()-1) throw(eEndOfBuffer);
	Uint8 ret = (Uint8) (*this)[pos];
	pos++;
	return ret;
}

//Uint16:
CBinBuffer & CBinBuffer::operator += (const Uint16 & i)
{
	operator+=(HIBYTE(i));
	operator+=(LOBYTE(i));
	return *this;
}

Uint16 CBinBuffer::getUint16(int unsigned &pos) const
{
	Uint16 ret = ((Uint16)getUint8(pos)) << 8;
	ret += ((Uint16)getUint8(pos));
	return ret;
}

//Uint32:
CBinBuffer & CBinBuffer::operator += (const Uint32 & i)
{
	operator+=((Uint8)((i & 0xFF000000) >> 24));
	operator+=((Uint8)((i & 0x00FF0000) >> 16));
	operator+=((Uint8)((i & 0x0000FF00) >> 8 ));
	operator+=((Uint8)( i & 0x000000FF       ));
	return *this;
}

Uint32 CBinBuffer::getUint32(int unsigned &pos) const
{
	Uint32 ret = ((Uint32)getUint8(pos)) << 24;
	ret += ((Uint32)getUint8(pos)) << 16;
	ret += ((Uint32)getUint8(pos)) << 8;
	ret += ((Uint32)getUint8(pos));
	return ret;
}

//String:
CBinBuffer & CBinBuffer::operator += (const CString & bs)
{
	operator+=((Uint8) bs.size());
	for (int unsigned i=0;i<bs.size();i++)
		operator+=((Uint8) bs[i]);

	return *this;
}

CString CBinBuffer::getCString(int unsigned &pos) const
{
	CString res;
	if (pos > this->size()-1) return ""; //throw(eEndOfBuffer);
	Uint8 ssize = getUint8(pos);
	if ((pos + ssize) > this->size()) return ""; //throw(eEndOfBuffer);
	for (int i=0;i<ssize;i++)
	{
		res.push_back(getUint8(pos));
	}

	return res;
}

//Float:
//I don't trust integer arithmetic near overflow border,
//so I forced float calculations
CBinBuffer & CBinBuffer::addFloat8(const float & v, float unit)
{
	return operator+=((Uint8)((v/unit) + (float)(0x7F)));
}

float CBinBuffer::getFloat8(int unsigned &pos, float unit) const
{
	return unit * ((float)(getUint8(pos)) - (float)(0x7F));
}

CBinBuffer & CBinBuffer::addFloat16(const float & v, float unit)
{
	return operator+=((Uint16)((v/unit) + (float)(0x7FFF)));
}

float CBinBuffer::getFloat16(int unsigned &pos, float unit) const
{
	return unit * ((float)(getUint16(pos)) - (float)(0x7FFF));
}

CBinBuffer & CBinBuffer::addFloat32(const float & v, float unit)
{
	//double is necessary because we need more than 24bit mantissa accuracy here
	return operator+=((Uint32)((double)(v/unit) + (double)(0x7FFFFFFF)));
}

float CBinBuffer::getFloat32(int unsigned &pos, float unit) const
{
	//double is necessary because we need more than 24bit mantissa accuracy here
	return unit * ((double)(getUint32(pos)) - (double)(0x7FFFFFFF));
}

//Vector:
CBinBuffer & CBinBuffer::addVector8(const CVector & v, float unit)
{
	addFloat8(v.x, unit);
	addFloat8(v.y, unit);
	addFloat8(v.z, unit);
	return *this;
}

CVector CBinBuffer::getVector8(int unsigned &pos, float unit) const
{
	CVector ret;
	ret.x = getFloat8(pos, unit);
	ret.y = getFloat8(pos, unit);
	ret.z = getFloat8(pos, unit);
	return ret;
}

CBinBuffer & CBinBuffer::addVector16(const CVector & v, float unit)
{
	addFloat16(v.x, unit);
	addFloat16(v.y, unit);
	addFloat16(v.z, unit);
	return *this;
}

CVector CBinBuffer::getVector16(int unsigned &pos, float unit) const
{
	CVector ret;
	ret.x = getFloat16(pos, unit);
	ret.y = getFloat16(pos, unit);
	ret.z = getFloat16(pos, unit);
	return ret;
}

CBinBuffer & CBinBuffer::addVector32(const CVector & v, float unit)
{
	addFloat32(v.x, unit);
	addFloat32(v.y, unit);
	addFloat32(v.z, unit);
	return *this;
}

CVector CBinBuffer::getVector32(int unsigned &pos, float unit) const
{
	CVector ret;
	ret.x = getFloat32(pos, unit);
	ret.y = getFloat32(pos, unit);
	ret.z = getFloat32(pos, unit);
	return ret;
}

//Matrix:
CBinBuffer & CBinBuffer::addMatrix8(const CMatrix & m, float unit)
{
	for(unsigned int i=0; i < 3; i++)
		for(unsigned int j=0; j < 3; j++)
			addFloat8(m.Element(i,j), unit);

	return *this;
}

CMatrix CBinBuffer::getMatrix8(int unsigned &pos, float unit) const
{
	CMatrix ret;
	for(unsigned int i=0; i < 3; i++)
		for(unsigned int j=0; j < 3; j++)
			ret.setElement(i, j, getFloat8(pos, unit));
	return ret;
}

CBinBuffer & CBinBuffer::addMatrix16(const CMatrix & m, float unit)
{
	for(unsigned int i=0; i < 3; i++)
		for(unsigned int j=0; j < 3; j++)
			addFloat16(m.Element(i,j), unit);

	return *this;
}

CMatrix CBinBuffer::getMatrix16(int unsigned &pos, float unit) const
{
	CMatrix ret;
	for(unsigned int i=0; i < 3; i++)
		for(unsigned int j=0; j < 3; j++)
			ret.setElement(i, j, getFloat16(pos, unit));
	return ret;
}

CString CBinBuffer::dump() const {
	CString res;
	for (int unsigned i=0;i<this->size();i++) {
		char c=(*this)[i];
		char s[10];
		if ( ((int (c) >= 97) && (int (c) <= 122)) ||
		((int (c) >= 65) && (int (c) <= 90))
		)
		{
			sprintf(s, "('%c' 0x%x)", c, (Uint8) c);
		} else {
			sprintf(s, "('?' 0x%x)", (Uint8) c);
		}
		res+=s;
	}
	return res;
}

CBinBuffer CBinBuffer::substr(const int unsigned pos, const int n) const {
	CBinBuffer res;
	if (pos <= size())
	{
		int len = n;
		if (len == -1) len = size() - pos;
		for (int unsigned i = pos;i<len+pos;i++)
			res.push_back((*this)[i]);
	}
	return res;
}

/*
char * CBinBuffer::raw_str() const {        // NOT zero terminated (use .dump().c_str() to print it)
	char * res = new char[this->size()+1];
	for (int unsigned i=0;i<this->size();i++) { res[i]= (char) (*this)[i]; }
	return (res);
}
*/
