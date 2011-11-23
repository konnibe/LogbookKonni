#pragma once
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/textfile.h> 
#include <wx/grid.h>

#define CREWFIELDS 13

class LogbookDialog;

class CrewList
{
public:
	CrewList(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT);
	~CrewList(void);

	void addCrew(wxGrid* grid, wxGrid* wake);
	void changeCrew(wxGrid* grid, int row, int col, int offset);
	void changeCrewWake(wxGrid* grid, int row, int col, int offset);
	void saveCSV(wxString path);
	void saveHTML(wxString path,wxString layout, bool mode);
	void saveODT(wxString path,wxString layout, bool mode);
	void saveXML(wxString path);
	void saveODS(wxString path);
	void backup(wxString path);
	void viewHTML(wxString path,wxString layout);
	void viewODT(wxString path,wxString layout);
	void deleteRow(int row);
	void saveData();
	void setLayoutLocation(wxString loc);

	wxString		layout_locn;
	wxString		layout;
	wxString		ODTLayout;

private:
	void loadData();
	wxString readLayout(wxString layoutFileName);
	wxString readLayoutODT(wxString layoutFileName);
	wxString replacePlaceholder(wxString html, wxString s, bool ngrid, int row, int col, bool mode);

	bool modified;

	wxTextFile* crewListFile;
	

private:
	wxGrid*			gridCrew;
	wxGrid*			gridWake;
	wxArrayString	crewFields;
	LogbookDialog*	dialog;
	wxString		data_locn;
	wxString 		html_locn;
	wxString		ODT_locn;

	enum fields{NAME,BIRTHNAME,FIRSTNAME,TITLE,BIRTHDATE,BIRTHPLACE,NATIONALITY,
				PASSPORT,EST_IN,EST_ON,ZIP,COUNTRY,TOWN,STREET};
	enum fieldsWake{LWNAME,LWFIRSTNAME,WAKESTART1,WAKEEND1,WAKESTART2,WAKEEND2,WAKESTART3,WAKEEND3};
};
