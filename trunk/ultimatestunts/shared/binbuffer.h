/***************************************************************************
                          binbuffer.h  -  binary buffer class
                             -------------------
    copyright            : (C) 2002 by bones
    email                : boesemar@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _CBinBuffer_H_
#define _CBinBuffer_H_

#include <vector>
namespace std {}
using namespace std;

#include "cstring.h"
#include "usmacros.h"

//Other types that should be added to the buffer:
#include "vector.h"
#include "matrix.h"


class CBinBuffer : public vector<Uint8>
{
public:

//	CBinBuffer & operator += (const char unsigned & c); //better use Uint8 ??
	CBinBuffer & operator += (const char unsigned * c);
	CBinBuffer & operator = (const char unsigned * c);
	CBinBuffer & operator += (const Uint8 & i);
	CBinBuffer & operator += (const Uint16 & i);
	CBinBuffer & operator += (const Uint32 & i);
	CBinBuffer & operator += (const CBinBuffer & bb);
	CBinBuffer & operator += (const CString & bs);
	CBinBuffer & addFloat8(const float & v, float unit=0.001);  //range -unit to unit;  8 bit precision
	CBinBuffer & addFloat16(const float & v, float unit=0.001); //range -unit to unit; 16 bit precision
	CBinBuffer & addFloat32(const float & v, float unit=0.001); //range -unit to unit; 32 bit precision
	CBinBuffer & addVector8(const CVector & v, float unit=0.001);
	CBinBuffer & addVector16(const CVector & v, float unit=0.001);
	CBinBuffer & addVector32(const CVector & v, float unit=0.001);
	CBinBuffer & addMatrix8(const CMatrix & m, float unit=1.0);
	CBinBuffer & addMatrix16(const CMatrix & m, float unit=1.0);

	Uint8 getUint8(int unsigned &pos) const;
	Uint16 getUint16(int unsigned &pos) const;
	Uint32 getUint32(int unsigned &pos) const;
	CString getCString(int unsigned &pos) const;
	CBinBuffer getBuffer(int unsigned &pos, unsigned int maxlen) const;
	float getFloat8(int unsigned &pos, float unit=0.001) const;  //range -unit to unit;  8 bit precision
	float getFloat16(int unsigned &pos, float unit=0.001) const; //range -unit to unit; 16 bit precision
	float getFloat32(int unsigned &pos, float unit=0.001) const; //range -unit to unit; 32 bit precision
	CVector getVector8(int unsigned &pos, float unit=0.001) const;
	CVector getVector16(int unsigned &pos, float unit=0.001) const;
	CVector getVector32(int unsigned &pos, float unit=0.001) const;
	CMatrix getMatrix8(int unsigned &pos, float unit = 1.0) const; //only for rotation matrices
	CMatrix getMatrix16(int unsigned &pos, float unit = 1.0) const; //only for rotation matrices

	CString dump() const;
	CBinBuffer substr(const int unsigned pos=0, const int n=-1) const;
};

#endif	//_CBinBuffer_H_

