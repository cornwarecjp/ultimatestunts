// CLconfig.h: interface for the CLconfig class.
// bones 2002
//////////////////////////////////////////////////////////////////////

#ifndef _LCONFIG_H
#define _LCONFIG_H

#include <vector>
namespace std {}
using namespace std;

#include <cstdio>
#include "cstring.h"




// -----
// CLCData - Class for CLConfig that holds the data of the INI file and provides search functions
// -----

class CLCData
{
private:

typedef struct {
		CString sec;
		CString field;
		CString val;
		CString line;
} d_data;

typedef vector<CString> CStringArray;

	vector<d_data> m_Data;

	void clear();

public:

	CStringArray CLCData::findAllFields(CString) const;
	CLCData();
	virtual ~CLCData();
	bool push(CString, CString, CString, CString);
	CString find(CString, CString) const;


  CStringArray findAll(CString) const;

};


// ------------------
//  CLCConfig, the INI file class
// ------------------
class CLConfig
{
private:
	CString m_szFilename;

	bool readFile();
	CLCData m_data;


public:

  CString getValue(const CString, const CString) const;
  vector<CString> getSection(CString) const;
  vector<CString> getFieldsInSection(CString) const;
	bool setFilename(CString);
	bool setArgs(int argc, char *argv[]);
	CLConfig();
	CLConfig(CString);
	CLConfig(int argc, char *argv[]);
	CLConfig(CString, int argc, char *argv[]);
	virtual ~CLConfig();

};

#endif
