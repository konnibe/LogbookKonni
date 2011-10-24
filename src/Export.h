#pragma once
#ifndef _EXPORT_H_
#define _EXPORT_H_

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/textfile.h>
#include <wx/generic/gridctrl.h>

class LogbookDialog;
class Boat;

class Export
{
public:
	Export(LogbookDialog *dialog);
	~Export(void);

	wxString readLayoutODT(wxString path,wxString layout);
	wxString readLayoutHTML(wxString path,wxString layoutFileName);

	bool cutInPartsODT(wxString odt, wxString* top, wxString* header, wxString* middle, wxString* bottom);
	bool cutInPartsHTML(wxString html, wxString* top, wxString* header, wxString* middle, wxString* bottom);

	bool writeToODT(wxTextFile* logFile, wxGrid* grid, wxString filenameOut,wxString filenameIn, 
		wxString top,wxString header,wxString middle,wxString bottom, bool mode);
	bool writeToHTML(wxTextFile* logFile, wxGrid* grid, wxString filenameOut,wxString filenameIn, 
		wxString top,wxString header,wxString middle,wxString bottom, bool mode);

	wxTextFile* setFiles(wxString* path, bool mode);
	virtual wxString setPlaceHolders(bool mode, wxGrid *grid, int row, wxString middle){ return _T("");}
	wxString replaceNewLine(bool mode, wxString str);

private:
	LogbookDialog* dialog;

};
#endif