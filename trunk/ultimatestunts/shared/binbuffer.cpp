/***************************************************************************
                          binbuffer.cpp  -  description
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
#include "binbuffer.h"

//Code coming from binbuffer.h
//Originally written by bones

CBinBuffer & CBinBuffer::operator += (const char unsigned & c) {
	this->push_back((Uint8) c); 	
	return (*this);
}

CBinBuffer & CBinBuffer::operator += (const char unsigned * c) {                // c-string
	for (int i=0; c[i] != 0; i++) { this->push_back((Uint8) c[i]); } 	
	return (*this);
}

CBinBuffer & CBinBuffer::operator = (const char unsigned * c) {
	this->empty();
	(*this)+=c;
	return (*this);
}


CBinBuffer & CBinBuffer::operator += (const Uint16 & i) {
	this->push_back(HIBYTE(i));
	this->push_back(LOBYTE(i));
	return (*this);
}

CBinBuffer & CBinBuffer::operator += (const CBinBuffer & bb) {
	for (int unsigned i=0;i<bb.size();i++) this->push_back(bb[i]);   // FIXME: slow
	return (*this);
}

CBinBuffer & CBinBuffer::operator += (const CString & bs) {
	this->push_back((Uint8) bs.size());
	for (int unsigned i=0;i<bs.size();i++) {
		this->push_back((Uint8) bs[i]);
	}
  return (*this);
}

Uint8 CBinBuffer::getUint8(const int unsigned pos=0, int *newpos = NULL) const {
	if (pos > this->size()-1) throw(eEndOfBuffer);
	if (newpos != NULL) *newpos = pos+1;
	return ((Uint8) (*this)[pos]);
}

Uint16 CBinBuffer::getUint16(const int unsigned pos=0, int *newpos = NULL) const {
	if (pos > this->size()-2) throw(eEndOfBuffer);
	if (newpos !=NULL) *newpos = pos+2;
	return (((*this)[pos] << 8) + (*this)[pos+1]);
}

CString & CBinBuffer::getCString(const int unsigned pos=0, int *newpos = NULL) const {
	CString *res = new CString();
	if (pos > this->size()-1) throw(eEndOfBuffer);
	Uint8 ssize = (*this)[pos];
	if ((pos + ssize) > this->size()-1) throw(eEndOfBuffer);
	for (int i=0;i<ssize;i++) {
		res->push_back((*this)[pos+1+i]);
	}
	if (newpos !=NULL) *newpos = pos+1+ssize;
	return (*res);
}
   	
CString & CBinBuffer::dump() const {
	CString *res = new CString();
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
		*res+=s;
	}
	return (*res);
}

CBinBuffer & CBinBuffer::substr(const int unsigned pos = 0, const int n = -1) const {
	CBinBuffer *res = new CBinBuffer();
	if (pos <= this->size()) {
		int len = n;
		if (len == -1) { len = this->size() -pos; }
		for (int unsigned i = pos;i<len+pos;i++) {
			res->push_back((*this)[i]);
		}
	}
	return (*res);
}

char * CBinBuffer::raw_str() const {        // NOT zero terminated (use .dump().c_str() to print it)
	char * res = new char[this->size()+1];
	for (int unsigned i=0;i<this->size();i++) { res[i]= (char) (*this)[i]; }
	return (res);
}
