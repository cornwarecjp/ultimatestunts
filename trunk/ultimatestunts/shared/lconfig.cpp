//////////////////////////////////////////////////////////////////////
// CLconfig.cpp: implementation of the CLconfig class.
// bones 2002
//
//////////////////////////////////////////////////////////////////////


#include "lconfig.h"

CLConfig *theMainConfig = NULL;


///////////////////
// CLCData class implementation (helper class)
///////////////////

void CLCData::clear() {
 m_Data.clear();
}

CLCData::CLCData() {
 clear();
}

CLCData::~CLCData()
{
}

CLCData::CStringArray CLCData::findAll(CString sec) const
{

	  CStringArray res;
      d_data d;

	  for (unsigned int i=0;i < m_Data.size(); i++) {
      	  d = m_Data[i];

		  if  (d.sec == sec)  {
			  res.push_back(d.line);
		  }
	}
   return (res);
}


CString CLCData::find(CString sec, CString field) const
{
	  d_data d;
	  for (unsigned int i=0;i < m_Data.size(); i++) {
      	  d = m_Data[i];
		  if ( (d.sec.toUpper() == sec.toUpper()) && (d.field.toUpper() == field.toUpper()) )  {
			 return (d.val);
		  }
	}
  return (CString(""));
}


CLCData::CStringArray CLCData::findAllFields(CString sec) const
{
  CStringArray a = findAll(sec);
  CStringArray res;
  for (int unsigned i=0;i<a.size();i++)
  {
		   int sep=a[i].find_first_of("=");
		
		   CString val;

		   CString field = a[i].subStr(0, sep);
		   field.Trim();
		   res.push_back(field);
  }
  return (res);
}


bool CLCData::push(CString sec, CString fie, CString val = "", CString line = "")
{
	d_data d;
	d.sec = sec;
	d.field = fie;
	d.val = val;
	d.line = line;

	m_Data.push_back(d);
	return (true);
}



/////////////////////////////////////
////
//// CLCConfig class implementation
////
////////////////////////////////////

CString CLConfig::getValue(CString section, CString field) const
{
 CString s = m_data.find(section, field);
 return (s);

}

vector<CString> CLConfig::getSection(CString sec) const
{
 return (m_data.findAll(sec));
}

vector<CString> CLConfig::getFieldsInSection(CString sec) const
{
 return (m_data.findAllFields(sec));
}

bool CLConfig::setFilename(CString fn)
{
    m_szFilename = fn;
	return readFile();

}

bool CLConfig::setArgs(int argc, char *argv[])
{
	//TODO: do something
	return true;
}

bool CLConfig::readFile(void)
{
	char buffer[512];
	CString cSection;

	strcpy(buffer,m_szFilename.c_str());

	FILE *f=fopen(m_szFilename.c_str(), "r");
	
	if (f == NULL) return (false);
	
	while (fgets(&buffer[0], 512, f) != NULL) {
		CString tbuf;
        CString s;

		tbuf.assign(buffer);
		tbuf.Trim();		
        if (tbuf.length() == 0) continue;			// comments or empty lines
		if ((tbuf[0] == '#') ||
			(tbuf[0] == '\'')) continue;


		if (tbuf[0] == '[') {						// check for new section
			int last = tbuf.find_last_of("]");
			if (last < 0) { fclose(f); return (false); }
			cSection = tbuf.subStr(1, last - 1);
		} else {									// sonst feld == wert
			int sep=tbuf.find_first_of("=");
			if(sep > 0)								//else it is not a valid line, ignore it
			{
				CString field;
				CString val;

				field = tbuf.subStr(0, sep);
				val = tbuf.subStr(sep+1);
				field.Trim();
				val.Trim();
				m_data.push(cSection, field, val, tbuf);
			}
		}
	}
	fclose(f);
	return (true);
}



CLConfig::CLConfig()
{
}

CLConfig::CLConfig(CString fn)
{
  this->setFilename(fn);
}

CLConfig::CLConfig(int argc, char *argv[])
{
  this->setArgs(argc, argv);
}

CLConfig::CLConfig(CString fn, int argc, char *argv[])
{
  this->setFilename(fn);
  this->setArgs(argc, argv);
}


CLConfig::~CLConfig()
{
}
