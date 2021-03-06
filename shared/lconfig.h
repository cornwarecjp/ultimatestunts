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
		CString section;
		CString field;
		CString value;
		CString line;
		CString metadata;
	} d_data;

	typedef vector<CString> CStringArray;

	vector<d_data> m_Data;

	void clear();

public:

	CStringArray findAllSections() const;
	CStringArray findAllFields(CString) const;

	CLCData();
	virtual ~CLCData();
	bool push(
		const CString &section,
		const CString &field,
		const CString &value,
		const CString &line,
		const CString &metadata=""
		);

	CString find(CString section, CString field) const;
	CString findMeta(CString section, CString field) const;

	void set(CString, CString, CString);

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
	bool writeFile();

	CLCData m_data;


public:
	void setValue(const CString &, const CString &, const CString &);

	CString getValue(const CString &section, const CString &field) const;
	CString getMetaData(const CString &section, const CString &field) const;
	vector<CString> getSections() const;
	vector<CString> getSection(CString) const;
	vector<CString> getFieldsInSection(CString) const;
	bool setFilename(CString);
	bool setArgs(int argc, char *argv[]);

	bool saveFile(const CString &fn = "");

	CLConfig();
	CLConfig(CString);
	CLConfig(int argc, char *argv[]);
	CLConfig(CString, int argc, char *argv[]);
	virtual ~CLConfig();

};

extern CLConfig *theMainConfig;

#endif
